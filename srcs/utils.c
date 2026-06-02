#include "../includes/codexion.h"

/*
** get_time_ms : retourne le timestamp actuel en millisecondes
** On utilise gettimeofday() qui donne les secondes + microsecondes.
** On convertit : secondes * 1000 + microsecondes / 1000 = millisecondes.
*/
long long   get_time_ms(void)
{
    struct timeval  tv;

    gettimeofday(&tv, NULL);                    /* remplit tv.tv_sec et tv.tv_usec */
    return ((long long)tv.tv_sec * 1000LL       /* secondes → millisecondes */
        + (long long)tv.tv_usec / 1000LL);      /* microsecondes → millisecondes */
}

/*
** elapsed_ms : retourne le temps écoulé depuis 'start' en ms
** Utilisé pour afficher les timestamps relatifs au début de la simu.
*/
long long   elapsed_ms(long long start)
{
    return (get_time_ms() - start);
}

/*
** ft_usleep : dort pendant exactement 'ms' millisecondes
** Le usleep standard peut dormir un peu moins que demandé à cause de l'OS.
** On boucle donc jusqu'à ce que le temps réel soit écoulé.
** C'est crucial pour la précision du burnout (<= 10ms d'erreur).
*/
void    ft_usleep(long long ms)
{
    long long   start;

    start = get_time_ms();
    while (get_time_ms() - start < ms)          /* tant que le temps n'est pas écoulé */
        usleep(100);                            /* petite attente de 100 microsecondes */
}

/*
** ft_atoi_strict : parse un entier depuis une string, retourne 0 si invalide
** 'out' reçoit la valeur parsée.
** Retourne 1 si succès, 0 si la string contient un non-chiffre ou est vide.
** On rejette aussi les négatifs (premier char '-').
*/
int ft_atoi_strict(const char *str, long long *out)
{
    int     i;
    long long   result;

    if (!str || !*str)                          /* string vide ou NULL → invalide */
        return (0);
    if (str[0] == '-')                          /* négatif → invalide */
        return (0);
    i = 0;
    result = 0;
    while (str[i])
    {
        if (str[i] < '0' || str[i] > '9')      /* caractère non numérique → invalide */
            return (0);
        result = result * 10 + (str[i] - '0'); /* accumule le chiffre */
        i++;
    }
    *out = result;                              /* stocke le résultat */
    return (1);                                 /* succès */
}
