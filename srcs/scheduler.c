#include "../includes/codexion.h"


static int  heap_compare(t_heap *h, t_request *a, t_request *b)
{
    if (h->mode == FIFO)
    {
        /* FIFO : le plus ancien timestamp gagne */
        if (a->timestamp != b->timestamp)
            return (a->timestamp < b->timestamp);
        /* Tie-breaker : id plus petit gagne (déterministe) */
        return (a->coder_id < b->coder_id);
    }
    else /* EDF */
    {
        /* EDF : la deadline la plus proche gagne */
        if (a->deadline != b->deadline)
            return (a->deadline < b->deadline);
        /* Tie-breaker : id plus petit gagne (déterministe) */
        return (a->coder_id < b->coder_id);
    }
}

t_heap  *heap_create(int capacity, int mode)
{
    t_heap  *h;

    h = malloc(sizeof(t_heap));
    if (!h)
        return (NULL);
    h->data = malloc(sizeof(t_request *) * capacity);
    if (!h->data)
    {
        free(h);
        return (NULL);
    }
    h->size = 0;
    h->capacity = capacity;
    h->mode = mode;
    return (h);
}

static void heap_swap(t_heap *h, int i, int j)
{
    t_request   *tmp;

    tmp = h->data[i];
    h->data[i] = h->data[j];
    h->data[j] = tmp;
}

/*
** heap_push : ajoute une requête dans le heap
** On l'ajoute à la fin, puis on "remonte" (sift up) jusqu'à
** ce que la propriété du heap soit respectée.
**
** Sift up : tant que le nœud courant est "meilleur" que son parent,
** on les échange et on remonte.
*/
void    heap_push(t_heap *h, t_request *req)
{
    int i;
    int parent;

    if (h->size >= h->capacity)                 /* heap plein, ne devrait pas arriver */
        return ;
    h->data[h->size] = req;                     /* on ajoute à la fin */
    i = h->size;                                /* index du nouvel élément */
    h->size++;

    /* Sift up : remonte l'élément jusqu'à sa bonne place */
    while (i > 0)
    {
        parent = (i - 1) / 2;                  /* index du parent */
        if (heap_compare(h, h->data[i], h->data[parent]))
        {
            heap_swap(h, i, parent);            /* échange avec le parent */
            i = parent;                         /* on continue vers le haut */
        }
        else
            break ;                             /* bonne place trouvée */
    }
}

/*
** heap_pop : retire et retourne l'élément prioritaire (la racine)
** On place le dernier élément à la racine, on réduit la taille,
** puis on "descend" (sift down) pour rétablir la propriété du heap.
**
** Sift down : on échange avec le plus petit des deux enfants
** tant que ce fils est "meilleur" que le nœud courant.
*/
t_request   *heap_pop(t_heap *h)
{
    t_request   *top;
    int         i;
    int         left;
    int         right;
    int         best;

    if (h->size == 0)                           /* heap vide */
        return (NULL);
    top = h->data[0];                           /* sauvegarde la racine (la réponse) */
    h->size--;
    h->data[0] = h->data[h->size];             /* déplace le dernier à la racine */

    /* Sift down : descend le nouvel élément racine */
    i = 0;
    while (1)
    {
        left = 2 * i + 1;                       /* index enfant gauche */
        right = 2 * i + 2;                      /* index enfant droit */
        best = i;                               /* on suppose que le courant est le meilleur */

        /* Vérifie si l'enfant gauche est meilleur */
        if (left < h->size && heap_compare(h, h->data[left], h->data[best]))
            best = left;
        /* Vérifie si l'enfant droit est meilleur */
        if (right < h->size && heap_compare(h, h->data[right], h->data[best]))
            best = right;

        if (best == i)                          /* aucun enfant meilleur → on s'arrête */
            break ;
        heap_swap(h, i, best);                  /* échange avec le meilleur enfant */
        i = best;                               /* continue vers le bas */
    }
    return (top);
}

void    heap_destroy(t_heap *h)
{
    if (!h)
        return ;
    free(h->data);
    free(h);
}
