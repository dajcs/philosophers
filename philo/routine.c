/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 13:53:27 by anemet            #+#    #+#             */
/*   Updated: 2025/07/27 08:01:44 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	philo_eat(t_philo *philo)
{
	pthread_mutex_lock(philo->left_fork);
	print_status(philo, "has taken a fork");
	if (philo->prog->num_philos == 1)
	{
		pthread_mutex_unlock(philo->left_fork);
		precise_sleep(philo->prog->time_to_die * 2, philo->prog);
		return ;
	}
	pthread_mutex_lock(philo->right_fork);
	print_status(philo, "has taken a fork");
	philo->last_meal_time = get_time();
	print_status(philo, "is eating");
	philo->eat_count++;
	precise_sleep(philo->prog->time_to_eat, philo->prog);
	pthread_mutex_unlock(philo->right_fork);
	pthread_mutex_unlock(philo->left_fork);
}

void	*philosopher_routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	if (philo->id % 2 == 0)
		usleep(5000);
	while (!philo->prog->stop_simulation)
	{
		philo_eat(philo);
		if (philo->prog->stop_simulation)
			break ;
		print_status(philo, "is sleeping");
		precise_sleep(philo->prog->time_to_sleep, philo->prog);
		if (philo->prog->stop_simulation)
			break ;
		print_status(philo, "is thinking");
	}
	return (NULL);
}
