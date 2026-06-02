/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgranier <bgranier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 10:59:37 by bgranier          #+#    #+#             */
/*   Updated: 2026/06/02 11:57:49 by bgranier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	check_running(t_sim *sim)
{
	int	running;

	pthread_mutex_lock(&sim->running_mutex);
	running = sim->running;
	pthread_mutex_unlock(&sim->running_mutex);
	return (running);
}

static void	print_log(t_sim *sim, long long ts, int id, const char *msg)
{
	pthread_mutex_lock(&sim->log_mutex);
	printf("%lld %d %s\n", ts, id, msg);
	pthread_mutex_unlock(&sim->log_mutex);
}

void	ft_log(t_sim *sim, int coder_id, const char *msg)
{
	long long	ts;

	if (!check_running(sim))
		return ;
	ts = elapsed_ms(sim->start_time);
	print_log(sim, ts, coder_id, msg);
}

static void	set_stop_and_print(t_sim *sim, long long ts, int id)
{
	pthread_mutex_lock(&sim->log_mutex);
	printf("%lld %d burned out\n", ts, id);
	pthread_mutex_unlock(&sim->log_mutex);
}

void	ft_log_burnout(t_sim *sim, int coder_id)
{
	long long	ts;

	pthread_mutex_lock(&sim->running_mutex);
	if (!sim->running)
	{
		pthread_mutex_unlock(&sim->running_mutex);
		return ;
	}
	sim->running = 0;
	pthread_mutex_unlock(&sim->running_mutex);
	ts = elapsed_ms(sim->start_time);
	set_stop_and_print(sim, ts, coder_id);
}
