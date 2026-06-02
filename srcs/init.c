#include "../includes/codexion.h"


int parse_args(int ac, char **av, t_sim *sim)
{
    long long   vals[7];
    int         i;

    if (ac != 9)
    {
        fprintf(stderr, "Usage: %s nb_coders time_to_burnout time_to_compile "
            "time_to_debug time_to_refactor nb_compiles_required "
            "dongle_cooldown scheduler\n", av[0]);
        return (0);
    }

    i = 0;
    while (i < 7)
    {
        if (!ft_atoi_strict(av[i + 1], &vals[i]))
        {
            fprintf(stderr, "Error: invalid argument '%s'\n", av[i + 1]);
            return (0);
        }
        i++;
    }

    if (vals[0] < 1)
    {
        fprintf(stderr, "Error: number_of_coders must be >= 1\n");
        return (0);
    }

    if (strcmp(av[8], "fifo") != 0 && strcmp(av[8], "edf") != 0)
    {
        fprintf(stderr, "Error: scheduler must be 'fifo' or 'edf'\n");
        return (0);
    }

    sim->nb_coders             = (int)vals[0];
    sim->time_to_burnout       = vals[1];
    sim->time_to_compile       = vals[2];
    sim->time_to_debug         = vals[3];
    sim->time_to_refactor      = vals[4];
    sim->nb_compiles_required  = (int)vals[5];
    sim->dongle_cooldown       = vals[6];
    if (strcmp(av[8], "fifo") == 0)
        sim->scheduler = FIFO;
    else
        sim->scheduler = EDF;

    return (1);
}

int init_sim(t_sim *sim)
{
    int n;
    int i;

    n = sim->nb_coders;

    pthread_mutex_init(&sim->log_mutex, NULL);
    pthread_mutex_init(&sim->running_mutex, NULL);
    sim->running = 1;
    sim->start_time = get_time_ms();

    sim->compile_count = malloc(sizeof(int) * n);
    sim->last_compile_start = malloc(sizeof(long long) * n);
    sim->dongles = malloc(sizeof(t_dongle) * n);

    if (!sim->compile_count || !sim->last_compile_start || !sim->dongles)
        return (0);

    i = 0;
    while (i < n)
    {
        sim->compile_count[i] = 0;
        sim->last_compile_start[i] = sim->start_time;
        i++;
    }

    i = 0;
    while (i < n)
    {
        pthread_mutex_init(&sim->dongles[i].mutex, NULL);
        pthread_cond_init(&sim->dongles[i].cond, NULL);
        sim->dongles[i].in_use = 0;
        sim->dongles[i].available_at = 0;
        sim->dongles[i].queue = heap_create(n, sim->scheduler);
        if (!sim->dongles[i].queue)
            return (0);
        i++;
    }

    return (1);
}

void    cleanup_sim(t_sim *sim)
{
    int i;

    i = 0;
    while (i < sim->nb_coders)
    {
        heap_destroy(sim->dongles[i].queue);
        pthread_mutex_destroy(&sim->dongles[i].mutex);
        pthread_cond_destroy(&sim->dongles[i].cond);
        i++;
    }
    free(sim->dongles);
    free(sim->compile_count);
    free(sim->last_compile_start);
    pthread_mutex_destroy(&sim->log_mutex);
    pthread_mutex_destroy(&sim->running_mutex);
}
