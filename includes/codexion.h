#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>

# define FIFO 0
# define EDF  1

typedef struct s_request
{
    int             coder_id;
    long long       timestamp;
    long long       deadline;
    int             granted;
    pthread_cond_t  cond;
}   t_request;

typedef struct s_heap
{
    t_request   **data;
    int         size;
    int         capacity;
    int         mode;
}   t_heap;

typedef struct s_dongle
{
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    t_heap          *queue;
    int             in_use;
    long long       available_at;
}   t_dongle;

typedef struct s_sim
{
    int         nb_coders;
    long long   time_to_burnout;
    long long   time_to_compile;
    long long   time_to_debug;
    long long   time_to_refactor;
    int         nb_compiles_required;
    long long   dongle_cooldown;
    int         scheduler;

    long long       start_time;
    int             running;
    pthread_mutex_t running_mutex;

    t_dongle    *dongles;
    int         *compile_count;
    long long   *last_compile_start;

    pthread_mutex_t log_mutex;
}   t_sim;

typedef struct s_coder
{
    int     id;
    t_sim   *sim;
}   t_coder;


/* utils.c */
long long   get_time_ms(void);
long long   elapsed_ms(long long start);
void        ft_usleep(long long ms);
int         ft_atoi_strict(const char *str, long long *out);

/* log.c */
void        ft_log(t_sim *sim, int coder_id, const char *msg);
void        ft_log_burnout(t_sim *sim, int coder_id);

/* scheduler.c */
t_heap      *heap_create(int capacity, int mode);
void        heap_push(t_heap *h, t_request *req);
t_request   *heap_pop(t_heap *h);
void        heap_destroy(t_heap *h);

/* dongle.c */
void        dongle_acquire(t_sim *sim, int dongle_idx, t_request *req);
void        dongle_release(t_sim *sim, int dongle_idx);

/* coder.c */
void        *coder_routine(void *arg);

/* monitor.c */
void        *monitor_routine(void *arg);

/* init.c */
int         parse_args(int ac, char **av, t_sim *sim);
int         init_sim(t_sim *sim);
void        cleanup_sim(t_sim *sim);

#endif