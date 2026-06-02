/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgranier <bgranier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 09:37:10 by bgranier          #+#    #+#             */
/*   Updated: 2026/06/02 12:38:42 by bgranier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_running(t_sim *sim)
{
	int	val;

	pthread_mutex_lock(&sim->running_mutex);
	val = sim->running;
	pthread_mutex_unlock(&sim->running_mutex);
	return (val);
}

void	*coder_routine(void *arg)
{
	t_coder	*c;
	t_sim	*sim;
	int		idx;

	c = (t_coder *)arg;
	sim = c->sim;
	idx = c->id - 1;
	ft_usleep((c->id - 1) * 2);
	while (is_running(sim))
	{
		if (!coder_compile(c))
			break ;
		if (!is_running(sim)
			|| sim->compile_count[idx] >= sim->nb_compiles_required)
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

static void	destroy_requests(t_request *l, t_request *r)
{
	pthread_cond_destroy(&l->cond);
	pthread_cond_destroy(&r->cond);
}

int	coder_compile(t_coder *c)
{
	t_sim		*sim;
	t_request	l;
	t_request	r;

	sim = c->sim;
	init_requests(c, &l, &r);
	if (sim->nb_coders == 1)
		return (one_coder_case(sim, &l));
	pthread_cond_init(&l.cond, NULL);
	pthread_cond_init(&r.cond, NULL);
	if (c->id < sim->nb_coders)
	{
		if (!take_dongles_normal(c, &l, &r))
			return (0);
	}
	else
	{
		if (!take_dongles_last(c, &l, &r))
			return (0);
	}
	do_compile(c);
	dongle_release(sim, c->id % sim->nb_coders);
	dongle_release(sim, c->id - 1);
	destroy_requests(&l, &r);
	return (1);
}
