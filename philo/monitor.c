/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 13:53:01 by anemet            #+#    #+#             */
/*   Updated: 2025/07/25 14:07:23 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	check_if_all_ate(t_program *prog)
{
	int	i;
	int	full_philos;

	i = 0;
	full_philos = 0;
	if (prog->num_must_eat == -1)
		return (0);
	while (i < prog->num_philos)
	{
		pthread_mutex_lock(&prog->philos[i].meal_lock);
		if (prog->philos[i].eat_count >= prog->num_must_eat)
			full_philos++;
		pthread_mutex_unlock(&prog->philos[i].meal_lock);
		i++;
	}
	if (full_philos == prog->num_philos)
	{
		pthread_mutex_lock(&prog->stop_lock);
		prog->stop_simulation = 1;
		pthread_mutex_unlock(&prog->stop_lock);
		return (1);
	}
	return (0);
}

static int	check_if_dead(t_philo *philo)
{
	long long	time;

	pthread_mutex_lock(&philo->meal_lock);
	time = get_time();
	if ((time - philo->last_meal_time) > philo->prog->time_to_die)
	{
		pthread_mutex_unlock(&philo->meal_lock);
		pthread_mutex_lock(&philo->prog->stop_lock);
		philo->prog->stop_simulation = 1;
		pthread_mutex_unlock(&philo->prog->stop_lock);
		print_status(philo, "died");
		return (1);
	}
	pthread_mutex_unlock(&philo->meal_lock);
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_program	*prog;
	int			i;

	prog = (t_program *)arg;
	while (1)
	{
		i = 0;
		while (i < prog->num_philos)
		{
			if (check_if_dead(&prog->philos[i]))
				return (NULL);
			i++;
		}
		if (check_if_all_ate(prog))
			return (NULL);
		usleep(100);
	}
	return (NULL);
}
