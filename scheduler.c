/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgranier <bgranier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 11:48:39 by bgranier          #+#    #+#             */
/*   Updated: 2026/06/02 11:58:10 by bgranier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	scheduler_compare(t_heap *h, t_request *a, t_request *b)
{
	if (h->mode == FIFO)
	{
		if (a->timestamp != b->timestamp)
			return (a->timestamp < b->timestamp);
		return (a->coder_id < b->coder_id);
	}
	else
	{
		if (a->deadline != b->deadline)
			return (a->deadline < b->deadline);
		return (a->coder_id < b->coder_id);
	}
}

t_heap	*heap_create(int capacity, int mode)
{
	t_heap	*h;

	h = malloc(sizeof(t_heap));
	if (!h)
		return (NULL);
	h->data = malloc(sizeof(t_request *) * capacity);
	if (!h->data)
		return (free(h), NULL);
	h->size = 0;
	h->capacity = capacity;
	h->mode = mode;
	return (h);
}

void	heap_destroy(t_heap *h)
{
	if (!h)
		return ;
	free(h->data);
	free(h);
}
