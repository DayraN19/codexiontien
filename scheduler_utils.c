/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgranier <bgranier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 11:49:50 by bgranier          #+#    #+#             */
/*   Updated: 2026/06/02 11:58:06 by bgranier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	heap_swap(t_heap *h, int i, int j)
{
	t_request	*tmp;

	tmp = h->data[i];
	h->data[i] = h->data[j];
	h->data[j] = tmp;
}

void	heap_push(t_heap *h, t_request *req)
{
	int	i;
	int	parent;

	if (h->size >= h->capacity)
		return ;
	h->data[h->size] = req;
	i = h->size;
	h->size++;
	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (scheduler_compare(h, h->data[i], h->data[parent]))
		{
			heap_swap(h, i, parent);
			i = parent;
		}
		else
			break ;
	}
}

static int	get_best_child(t_heap *h, int i, int left, int right)
{
	int	best;

	best = i;
	if (left < h->size
		&& scheduler_compare(h, h->data[left], h->data[best]))
		best = left;
	if (right < h->size
		&& scheduler_compare(h, h->data[right], h->data[best]))
		best = right;
	return (best);
}

t_request	*heap_pop(t_heap *h)
{
	t_request	*top;
	int			i;
	int			left;
	int			right;
	int			best;

	if (h->size == 0)
		return (NULL);
	top = h->data[0];
	h->size--;
	h->data[0] = h->data[h->size];
	i = 0;
	while (1)
	{
		left = 2 * i + 1;
		right = 2 * i + 2;
		best = get_best_child(h, i, left, right);
		if (best == i)
			break ;
		heap_swap(h, i, best);
		i = best;
	}
	return (top);
}
