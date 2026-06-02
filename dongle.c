/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgranier <bgranier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 10:37:14 by bgranier          #+#    #+#             */
/*   Updated: 2026/06/02 12:34:42 by bgranier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	set_timespec(struct timespec *ts, long long wait_ms)
{
	clock_gettime(CLOCK_REALTIME, ts);
	ts->tv_sec += wait_ms / 1000;
	ts->tv_nsec += (wait_ms % 1000) * 1000000LL;
	if (ts->tv_nsec >= 1000000000LL)
	{
		ts->tv_sec++;
		ts->tv_nsec -= 1000000000LL;
	}
}

static void	try_acquire(t_dongle *d, t_request *req, long long now)
{
	long long		wait_ms;
	struct timespec	ts;

	if (d->queue->size > 0 && d->queue->data[0] == req && !d->in_use)
	{
		if (now >= d->available_at)
		{
			heap_pop(d->queue);
			d->in_use = 1;
			req->granted = 1;
			if (d->queue->size > 0)
				pthread_cond_signal(&d->queue->data[0]->cond);
			return ;
		}
		wait_ms = d->available_at - now;
		set_timespec(&ts, wait_ms);
		pthread_cond_timedwait(&req->cond, &d->mutex, &ts);
	}
	else
	{
		pthread_cond_wait(&req->cond, &d->mutex);
	}
}

void	dongle_acquire(t_sim *sim, int dongle_idx, t_request *req)
{
	t_dongle	*d;
	long long	now;

	d = &sim->dongles[dongle_idx];
	pthread_mutex_lock(&d->mutex);
	pthread_cond_init(&req->cond, NULL);
	req->granted = 0;
	heap_push(d->queue, req);
	while (!req->granted)
	{
		now = get_time_ms();
		try_acquire(d, req, now);
	}
	pthread_mutex_unlock(&d->mutex);
}

void	dongle_release(t_sim *sim, int dongle_idx)
{
	t_dongle	*d;

	d = &sim->dongles[dongle_idx];
	pthread_mutex_lock(&d->mutex);
	d->in_use = 0;
	d->available_at = get_time_ms() + sim->dongle_cooldown;
	if (d->queue->size > 0)
		pthread_cond_signal(&d->queue->data[0]->cond);
	pthread_mutex_unlock(&d->mutex);
}
