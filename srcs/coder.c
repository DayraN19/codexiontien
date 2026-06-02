#include "../includes/codexion.h"

/*
** ===== THREAD CODER =====
**
** Chaque coder est un thread qui boucle indéfiniment sur :
**   1. Demander les deux dongles (gauche et droite)
**   2. Compiler (dormir time_to_compile ms)
**   3. Relâcher les deux dongles
**   4. Debugger (dormir time_to_debug ms)
**   5. Refactorer (dormir time_to_refactor ms)
**   Recommencer depuis le début.
**
** Topologie des dongles (cercle) :
**   - coder i (1-indexed) a : left = dongle[i-1], right = dongle[i % N]
**   - coder 1 : left=dongle[0], right=dongle[1]
**   - coder N : left=dongle[N-1], right=dongle[0]
**
** Prévention du deadlock :
**   Si tous prennent le gauche puis le droit → deadlock circulaire.
**   Solution : le dernier coder prend droite puis gauche (Dijkstra).
**   Cela brise la circularité.
**
** Cas spécial N=1 :
**   Un seul dongle, un seul coder → il ne peut jamais avoir 2 dongles
**   → il burnera toujours. Le monitor le détecte.
*/

static int  is_running(t_sim *sim)
{
    int val;

    pthread_mutex_lock(&sim->running_mutex);
    val = sim->running;
    pthread_mutex_unlock(&sim->running_mutex);
    return (val);
}

static int  coder_compile(t_coder *c)
{
    t_sim       *sim;
    int         id;
    int         left;
    int         right;
    t_request   req_left;
    t_request   req_right;

    sim = c->sim;
    id = c->id;

    left  = id - 1;
    right = id % sim->nb_coders;

    req_left.coder_id  = id;
    req_left.timestamp = get_time_ms();
    req_left.deadline  = sim->last_compile_start[id - 1] + sim->time_to_burnout;
    req_right.coder_id  = id;
    req_right.timestamp = get_time_ms();
    req_right.deadline  = sim->last_compile_start[id - 1] + sim->time_to_burnout;

    if (sim->nb_coders == 1)
    {
        pthread_cond_init(&req_left.cond, NULL);
        ft_usleep(sim->time_to_burnout + 100);  /* attend le burnout */
        pthread_cond_destroy(&req_left.cond);
        return (0);
    }

    pthread_cond_init(&req_left.cond, NULL);
    pthread_cond_init(&req_right.cond, NULL);

    if (id < sim->nb_coders)
    {
        if (!is_running(sim))
        {
            pthread_cond_destroy(&req_left.cond);
            pthread_cond_destroy(&req_right.cond);
            return (0);
        }
        dongle_acquire(sim, left, &req_left);
        if (!is_running(sim))
        {
            dongle_release(sim, left);
            pthread_cond_destroy(&req_left.cond);
            pthread_cond_destroy(&req_right.cond);
            return (0);
        }
        ft_log(sim, id, "has taken a dongle");

        dongle_acquire(sim, right, &req_right);
        if (!is_running(sim))
        {
            dongle_release(sim, right);
            dongle_release(sim, left);
            pthread_cond_destroy(&req_left.cond);
            pthread_cond_destroy(&req_right.cond);
            return (0);
        }
        ft_log(sim, id, "has taken a dongle");
    }
    else
    {
        if (!is_running(sim)) { pthread_cond_destroy(&req_left.cond); pthread_cond_destroy(&req_right.cond); return (0); }
        dongle_acquire(sim, right, &req_right);
        if (!is_running(sim)) { dongle_release(sim, right); pthread_cond_destroy(&req_left.cond); pthread_cond_destroy(&req_right.cond); return (0); }
        ft_log(sim, id, "has taken a dongle");

        dongle_acquire(sim, left, &req_left);
        if (!is_running(sim))
        {
            dongle_release(sim, left);
            dongle_release(sim, right);
            pthread_cond_destroy(&req_left.cond);
            pthread_cond_destroy(&req_right.cond);
            return (0);
        }
        ft_log(sim, id, "has taken a dongle");
    }

    /* Les deux dongles sont acquis → on compile */
    sim->last_compile_start[id - 1] = get_time_ms();
    ft_log(sim, id, "is compiling");
    ft_usleep(sim->time_to_compile);

    sim->compile_count[id - 1]++;

    dongle_release(sim, right);
    dongle_release(sim, left);

    pthread_cond_destroy(&req_left.cond);
    pthread_cond_destroy(&req_right.cond);

    return (1);
}

void    *coder_routine(void *arg)
{
    t_coder *c;
    t_sim   *sim;

    c = (t_coder *)arg;
    sim = c->sim;

    ft_usleep((c->id - 1) * 2);

    while (is_running(sim))
    {
        if (!coder_compile(c))
            break ;
        if (!is_running(sim))
            break ;

        ft_log(sim, c->id, "is debugging");
        ft_usleep(sim->time_to_debug);
        if (!is_running(sim))
            break ;

        ft_log(sim, c->id, "is refactoring");
        ft_usleep(sim->time_to_refactor);
    }

    return (NULL);
}
