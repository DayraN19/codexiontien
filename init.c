/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgranier <bgranier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 10:44:28 by bgranier          #+#    #+#             */
/*   Updated: 2026/06/02 11:57:45 by bgranier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	check_scheduler(char *av, t_sim *sim)
{
	if (strcmp(av, "fifo") == 0)
		sim->scheduler = FIFO;
	else if (strcmp(av, "edf") == 0)
		sim->scheduler = EDF;
	else
		return (0);
	return (1);
}

static int	parse_values(char **av, long long *vals)
{
	int	i;

	i = 0;
	while (i < 7)
	{
		if (!ft_atoi_strict(av[i + 1], &vals[i]))
			return (fprintf(stderr,
					"Error: invalid argument '%s'\n", av[i + 1]), 0);
		i++;
	}
	return (1);
}

static int	check_args(long long *vals, char **av, t_sim *sim)
{
	if (vals[0] < 1)
		return (fprintf(stderr,
				"Error: number_of_coders must be >= 1\n"), 0);
	if (!check_scheduler(av[8], sim))
		return (fprintf(stderr,
				"Error: scheduler must be 'fifo' or 'edf'\n"), 0);
	return (1);
}

static void	fill_sim(t_sim *sim, long long *v)
{
	sim->nb_coders = (int)v[0];
	sim->time_to_burnout = v[1];
	sim->time_to_compile = v[2];
	sim->time_to_debug = v[3];
	sim->time_to_refactor = v[4];
	sim->nb_compiles_required = (int)v[5];
	sim->dongle_cooldown = v[6];
}

int	parse_args(int ac, char **av, t_sim *sim)
{
	long long	vals[7];

	if (ac != 9)
		return (fprintf(stderr,
				"Usage: %s n_coders t_burnout t_compile "
				"t_debug t_refactor n_compiles "
				"dongle_cooldown scheduler\n", av[0]), 0);
	if (!parse_values(av, vals))
		return (0);
	if (!check_args(vals, av, sim))
		return (0);
	fill_sim(sim, vals);
	return (1);
}
