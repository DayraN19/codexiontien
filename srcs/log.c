#include "../includes/codexion.h"

/*
** ft_log : affiche un message d'état pour un coder de manière thread-safe
** On lock le log_mutex AVANT d'écrire pour être sûr que deux threads
** n'écrivent pas en même temps (ce qui mélangerait les lignes).
**
** Format : "timestamp_relatif coder_id message\n"
** Ex : "402 2 is compiling"
**
** On vérifie aussi que la simulation est toujours en cours avant de logger
** pour éviter des messages parasites après un burnout.
*/
void    ft_log(t_sim *sim, int coder_id, const char *msg)
{
    long long   ts;

    /* Vérifier que la simu tourne encore (lecture rapide sans lock pour perf) */
    pthread_mutex_lock(&sim->running_mutex);
    if (!sim->running)                              /* simu arrêtée → on n'affiche rien */
    {
        pthread_mutex_unlock(&sim->running_mutex);
        return ;
    }
    pthread_mutex_unlock(&sim->running_mutex);

    ts = elapsed_ms(sim->start_time);              /* timestamp relatif au début */

    pthread_mutex_lock(&sim->log_mutex);           /* on prend le verrou d'affichage */
    printf("%lld %d %s\n", ts, coder_id, msg);    /* affichage atomique depuis ce thread */
    pthread_mutex_unlock(&sim->log_mutex);         /* on libère le verrou */
}

/*
** ft_log_burnout : affiche le message de burnout et arrête la simulation
** Cas spécial : on doit locker log_mutex ET running_mutex ensemble
** pour garantir que le message de burnout est le DERNIER affiché.
** On set running=0 PENDANT qu'on tient le log_mutex pour éviter
** qu'un autre thread loge quelque chose après le burnout.
*/
void    ft_log_burnout(t_sim *sim, int coder_id)
{
    long long   ts;

    pthread_mutex_lock(&sim->running_mutex);
    if (!sim->running)                              /* déjà arrêtée (double burnout ?) */
    {
        pthread_mutex_unlock(&sim->running_mutex);
        return ;
    }
    sim->running = 0;                              /* on arrête la simulation */
    pthread_mutex_unlock(&sim->running_mutex);

    ts = elapsed_ms(sim->start_time);

    pthread_mutex_lock(&sim->log_mutex);
    printf("%lld %d burned out\n", ts, coder_id); /* message de burnout */
    pthread_mutex_unlock(&sim->log_mutex);
}