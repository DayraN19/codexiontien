#include "../includes/codexion.h"


int main(int ac, char **av)
{
    t_sim       sim;
    t_coder     *coders;
    pthread_t   *threads;
    pthread_t   monitor_thread;
    int         i;

    memset(&sim, 0, sizeof(t_sim));

    if (!parse_args(ac, av, &sim))
        return (1);

    if (!init_sim(&sim))
    {
        fprintf(stderr, "Error: initialization failed\n");
        return (1);
    }

    coders = malloc(sizeof(t_coder) * sim.nb_coders);
    threads = malloc(sizeof(pthread_t) * sim.nb_coders);
    if (!coders || !threads)
    {
        fprintf(stderr, "Error: malloc failed\n");
        cleanup_sim(&sim);
        free(coders);
        free(threads);
        return (1);
    }

    pthread_create(&monitor_thread, NULL, monitor_routine, &sim);

    i = 0;
    while (i < sim.nb_coders)
    {
        coders[i].id = i + 1;
        coders[i].sim = &sim;
        pthread_create(&threads[i], NULL, coder_routine, &coders[i]);
        i++;
    }
    pthread_join(monitor_thread, NULL);

    i = 0;
    while (i < sim.nb_coders)
    {
        pthread_join(threads[i], NULL);
        i++;
    }

    cleanup_sim(&sim);
    free(coders);
    free(threads);

    return (0);
}
