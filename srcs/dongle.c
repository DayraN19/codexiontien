#include "../includes/codexion.h"

void    dongle_acquire(t_sim *sim, int dongle_idx, t_request *req)
{
    t_dongle    *d;
    long long   now;
    long long   wait_ms;
    struct timespec ts_spec;

    d = &sim->dongles[dongle_idx];

    pthread_mutex_lock(&d->mutex);

    pthread_cond_init(&req->cond, NULL);
    req->granted = 0;

    heap_push(d->queue, req);

    /*
    ** Boucle d'attente : on attend que notre requête soit accordée (granted = 1)
    ** ET que le dongle soit disponible (cooldown écoulé, pas in_use).
    **
    ** La condition de réveil peut venir de dongle_release() ou du cooldown.
    */
    while (!req->granted)
    {
        /* Vérifie si on est le prochain de la file */
        if (d->queue->size > 0 && d->queue->data[0] == req && !d->in_use)
        {
            now = get_time_ms();
            if (now >= d->available_at)        /* cooldown terminé ? */
            {
                /* C'est notre tour ! On prend le dongle. */
                heap_pop(d->queue);            /* retire notre requête du heap */
                d->in_use = 1;                 /* marque le dongle comme utilisé */
                req->granted = 1;              /* on est accordé */

                /* Réveille le prochain en attente pour qu'il vérifie son tour */
                if (d->queue->size > 0)
                    pthread_cond_signal(&d->queue->data[0]->cond);
                break ;
            }
            else
            {
                /*
                ** Cooldown pas encore écoulé.
                ** On fait un pthread_cond_timedwait jusqu'à la fin du cooldown.
                ** timedwait prend un struct timespec en temps absolu (CLOCK_REALTIME).
                */
                wait_ms = d->available_at - now;    /* ms restantes de cooldown */
                clock_gettime(CLOCK_REALTIME, &ts_spec);
                ts_spec.tv_sec += wait_ms / 1000;
                ts_spec.tv_nsec += (wait_ms % 1000) * 1000000LL;
                /* Normalise tv_nsec (doit rester < 1 000 000 000) */
                if (ts_spec.tv_nsec >= 1000000000LL)
                {
                    ts_spec.tv_sec++;
                    ts_spec.tv_nsec -= 1000000000LL;
                }
                /* Attend jusqu'à la fin du cooldown ou un signal */
                pthread_cond_timedwait(&req->cond, &d->mutex, &ts_spec);
            }
        }
        else
        {
            /* Pas encore notre tour : on attend un signal de dongle_release */
            pthread_cond_wait(&req->cond, &d->mutex);
        }
    }

    pthread_mutex_unlock(&d->mutex);           /* on libère le verrou */
}

/*
** dongle_release : relâche le dongle et réveille le prochain coder en attente
**
** Appelé par le coder quand il a fini de compiler.
** On set in_use = 0, on calcule available_at (maintenant + cooldown),
** puis on pop la prochaine requête du heap et on signal sa cond.
*/
void    dongle_release(t_sim *sim, int dongle_idx)
{
    t_dongle    *d;

    d = &sim->dongles[dongle_idx];

    pthread_mutex_lock(&d->mutex);

    d->in_use = 0;                                          /* dongle libéré */
    d->available_at = get_time_ms() + sim->dongle_cooldown; /* cooldown démarre */

    /*
    ** Réveille le prochain coder en tête de file (s'il y en a un).
    ** Il se réveillera, verra que c'est son tour, et attendra
    ** la fin du cooldown si nécessaire via timedwait.
    */
    if (d->queue->size > 0)
        pthread_cond_signal(&d->queue->data[0]->cond);

    pthread_mutex_unlock(&d->mutex);
}