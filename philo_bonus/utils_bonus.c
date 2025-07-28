/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 18:12:54 by anemet            #+#    #+#             */
/*   Updated: 2025/07/28 16:15:04 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

long long	get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

int	ft_atoi(const char *str)
{
	int			i;
	long long	res;

	i = 0;
	res = 0;
	while (str[i] >= '0' && str[i] <= '9')
	{
		res = res * 10 + (str[i] - '0');
		i++;
	}
	return ((int)res);
}

/* All output goes through SEM_PRINT so lines never interleave */
void	print_status(t_prog *p, int id, char *status)
{
	long long	t;

	sem_wait(p->print);
	t = get_time() - p->start_time;
	printf("%lld %d %s\n", t, id, status);
	fflush(stdout);
	sem_post(p->print);
}

/* prints "died" message and keeps SEM_PRINT closed to keep it quiet */
void	print_death(t_prog *p, int id)
{
	long long	t;

	sem_wait(p->print);
	t = get_time() - p->start_time;
	printf("%lld %d died\n", t, id);
	fflush(stdout);
}

/* Sleep with sub-ms polling granularity to keep the death timing tight */
void	precise_sleep(long long ms)
{
	long long	start;

	start = get_time();
	while (1)
	{
		if (get_time() - start >= ms)
			break ;
		usleep(100);
	}
}
