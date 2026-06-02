/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgranier <bgranier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 11:55:31 by bgranier          #+#    #+#             */
/*   Updated: 2026/06/02 11:58:14 by bgranier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((long long)tv.tv_sec * 1000LL
		+ (long long)tv.tv_usec / 1000LL);
}

long long	elapsed_ms(long long start)
{
	return (get_time_ms() - start);
}

void	ft_usleep(long long ms)
{
	long long	start;

	start = get_time_ms();
	while (get_time_ms() - start < ms)
		usleep(100);
}

int	ft_atoi_strict(const char *str, long long *out)
{
	int			i;
	long long	result;

	if (!str || !*str)
		return (0);
	if (str[0] == '-')
		return (0);
	i = 0;
	result = 0;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		result = result * 10 + (str[i] - '0');
		i++;
	}
	*out = result;
	return (1);
}
