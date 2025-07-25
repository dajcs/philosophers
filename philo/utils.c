/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 14:32:33 by anemet            #+#    #+#             */
/*   Updated: 2025/07/25 14:45:22 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

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
	return (res);
}

void	print_status(t_philo *philo, char *status)
{
	long long	time;

	pthread_mutex_lock(&philo->prog->write_lock);
	if (!check_stop_flag(philo->prog))
	{
		time = get_time() - philo->prog->start_time;
		printf("%lld %d %s\n", time, philo->id, status);
	}
	pthread_mutex_unlock(&philo->prog->write_lock);
}

void	precise_sleep(long long ms, t_program *prog)
{
	long long	start;

	start = get_time();
	while (!check_stop_flag(prog))
	{
		if (get_time() - start >= ms)
			break ;
		usleep(50);
	}
}

int	check_stop_flag(t_program *prog)
{
	int	ret;

	pthread_mutex_lock(&prog->stop_lock);
	ret = prog->stop_simulation;
	pthread_mutex_unlock(&prog->stop_lock);
	return (ret);
}
