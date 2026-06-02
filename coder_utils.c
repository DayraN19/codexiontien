/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgranier <bgranier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 09:57:31 by bgranier          #+#    #+#             */
/*   Updated: 2026/06/02 11:57:19 by bgranier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	init_requests(t_coder *c, t_request *l, t_request *r)
{
	t_sim	*sim;

	sim = c->sim;
	l->coder_id = c->id;
	l->timestamp = get_time_ms();
	l->deadline = sim->last_compile_start[c->id - 1]
		+ sim->time_to_burnout;
	r->coder_id = c->id;
	r->timestamp = get_time_ms();
	r->deadline = sim->last_compile_start[c->id - 1]
		+ sim->time_to_burnout;
}

int	one_coder_case(t_sim *sim, t_request *l)
{
	pthread_cond_init(&l->cond, NULL);
	ft_usleep(sim->time_to_burnout + 100);
	pthread_cond_destroy(&l->cond);
	return (0);
}

void	do_compile(t_coder *c)
{
	t_sim	*sim;

	sim = c->sim;
	sim->last_compile_start[c->id - 1] = get_time_ms();
	ft_log(sim, c->id, "is compiling");
	ft_usleep(sim->time_to_compile);
	sim->compile_count[c->id - 1]++;
}

int	take_dongles_normal(t_coder *c, t_request *l, t_request *r)
{
	t_sim	*sim;
	int		left;
	int		right;

	sim = c->sim;
	left = c->id - 1;
	right = c->id % sim->nb_coders;
	dongle_acquire(sim, left, l);
	if (!is_running(sim))
		return (dongle_release(sim, left), 0);
	ft_log(sim, c->id, "has taken a dongle");
	dongle_acquire(sim, right, r);
	if (!is_running(sim))
	{
		dongle_release(sim, right);
		dongle_release(sim, left);
		return (0);
	}
	ft_log(sim, c->id, "has taken a dongle");
	return (1);
}

int	take_dongles_last(t_coder *c, t_request *l, t_request *r)
{
	t_sim	*sim;
	int		left;
	int		right;

	sim = c->sim;
	left = c->id - 1;
	right = c->id % sim->nb_coders;
	dongle_acquire(sim, right, r);
	if (!is_running(sim))
		return (dongle_release(sim, right), 0);
	ft_log(sim, c->id, "has taken a dongle");
	dongle_acquire(sim, left, l);
	if (!is_running(sim))
	{
		dongle_release(sim, left);
		dongle_release(sim, right);
		return (0);
	}
	ft_log(sim, c->id, "has taken a dongle");
	return (1);
}
