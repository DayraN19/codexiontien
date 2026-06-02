/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgranier <bgranier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 11:05:51 by bgranier          #+#    #+#             */
/*   Updated: 2026/06/02 11:58:01 by bgranier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	check_stop(t_sim *sim)
{
	pthread_mutex_lock(&sim->running_mutex);
	if (!sim->running)
	{
		pthread_mutex_unlock(&sim->running_mutex);
		return (1);
	}
	pthread_mutex_unlock(&sim->running_mutex);
	return (0);
}

static int	check_burnout_loop(t_sim *sim, long long now)
{
	int			i;
	long long	since_last;

	i = 0;
	while (i < sim->nb_coders)
	{
		since_last = now - sim->last_compile_start[i];
		if (since_last >= sim->time_to_burnout)
		{
			ft_log_burnout(sim, i + 1);
			return (1);
		}
		i++;
	}
	return (0);
}

int	check_all_done(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->nb_coders)
	{
		if (sim->compile_count[i] < sim->nb_compiles_required)
			return (0);
		i++;
	}
	return (1);
}

static void	stop_sim(t_sim *sim)
{
	pthread_mutex_lock(&sim->running_mutex);
	sim->running = 0;
	pthread_mutex_unlock(&sim->running_mutex);
}

void	*monitor_routine(void *arg)
{
	t_sim		*sim;
	long long	now;

	sim = (t_sim *)arg;
	while (1)
	{
		usleep(1000);
		if (check_stop(sim))
			break ;
		now = get_time_ms();
		if (check_burnout_loop(sim, now))
			return (NULL);
		if (check_all_done(sim))
		{
			stop_sim(sim);
			break ;
		}
	}
	return (NULL);
}
