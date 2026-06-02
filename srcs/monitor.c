#include "../includes/codexion.h"

/*
** ===== THREAD MONITOR =====
**
** Le monitor est un thread séparé qui tourne en parallèle des coders.
** Il a deux responsabilités :
**
** 1. DÉTECTER LES BURNOUTS
**    Toutes les ~1ms, il vérifie pour chaque coder :
**    si (now - last_compile_start[i]) >= time_to_burnout → burnout !
**    Il doit afficher le message dans les 10ms suivant le burnout réel.
**    C'est pourquoi on poll toutes les 1ms (largement dans la marge de 10ms).
**
** 2. DÉTECTER LA FIN NORMALE
**    Si tous les coders ont compilé au moins nb_compiles_required fois,
**    la simulation se termine normalement (sans burnout).
**
** Dès qu'une condition de fin est détectée, le monitor set running=0
** ce qui fait sortir les threads coders de leur boucle.
*/

void    *monitor_routine(void *arg)
{
    t_sim       *sim;
    int         i;
    long long   now;
    long long   since_last;
    int         all_done;

    sim = (t_sim *)arg;

    /* Boucle principale du monitor : tourne tant que la simu est active */
    while (1)
    {
        usleep(1000);                           /* poll toutes les 1ms */

        /* Vérifie si la simulation est encore en cours */
        pthread_mutex_lock(&sim->running_mutex);
        if (!sim->running)
        {
            pthread_mutex_unlock(&sim->running_mutex);
            break ;                             /* quelqu'un a déjà arrêté la simu */
        }
        pthread_mutex_unlock(&sim->running_mutex);

        now = get_time_ms();
        all_done = 1;                           /* on suppose que tout le monde a fini */

        i = 0;
        while (i < sim->nb_coders)
        {
            /* === VÉRIFICATION DU BURNOUT === */
            /*
            ** Temps depuis le dernier début de compile de ce coder.
            ** Si ce temps dépasse time_to_burnout → burnout !
            ** Note : last_compile_start[i] est initialisé à start_time
            ** donc le premier burnout possible est à t + time_to_burnout.
            */
            since_last = now - sim->last_compile_start[i];
            if (since_last >= sim->time_to_burnout)
            {
                /* BURNOUT DÉTECTÉ */
                ft_log_burnout(sim, i + 1);     /* i+1 car id est 1-indexed */
                return (NULL);                  /* monitor s'arrête */
            }

            /* === VÉRIFICATION DE LA FIN NORMALE === */
            if (sim->compile_count[i] < sim->nb_compiles_required)
                all_done = 0;                   /* ce coder n'a pas encore fini */

            i++;
        }

        /* Tous les coders ont atteint nb_compiles_required → fin normale */
        if (all_done)
        {
            pthread_mutex_lock(&sim->running_mutex);
            sim->running = 0;                   /* arrête la simulation */
            pthread_mutex_unlock(&sim->running_mutex);
            break ;
        }
    }

    return (NULL);
}
