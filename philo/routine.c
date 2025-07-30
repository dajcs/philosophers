/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 13:53:27 by anemet            #+#    #+#             */
/*   Updated: 2025/07/31 01:09:06 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

// fork global order: lower address first
static void	lock_forks(t_philo *ph)
{
	pthread_mutex_t	*first;
	pthread_mutex_t	*second;

	first = ph->right_fork;
	second = ph->left_fork;
	if (ph->left_fork < ph->right_fork)
	{
		first = ph->left_fork;
		second = ph->right_fork;
	}
	pthread_mutex_lock(first);
	print_status(ph, "has taken a fork");
	pthread_mutex_lock(second);
	print_status(ph, "has taken a fork");
}

static void	philo_eat(t_philo *philo)
{
	if (philo->prog->num_philos == 1)
	{
		pthread_mutex_lock(philo->left_fork);
		print_status(philo, "has taken a fork");
		precise_sleep(philo->prog->time_to_die * 2, philo->prog);
		pthread_mutex_unlock(philo->left_fork);
		return ;
	}
	lock_forks(philo);
	pthread_mutex_lock(&philo->meal_lock);
	philo->last_meal_time = get_time();
	print_status(philo, "is eating");
	pthread_mutex_unlock(&philo->meal_lock);
	precise_sleep(philo->prog->time_to_eat, philo->prog);
	pthread_mutex_unlock(philo->left_fork);
	pthread_mutex_unlock(philo->right_fork);
	pthread_mutex_lock(&philo->meal_lock);
	philo->eat_count++;
	pthread_mutex_unlock(&philo->meal_lock);
}

void	*philosopher_routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	if (philo->id % 2 == 0)
		usleep(5000);
	while (!get_stop(philo->prog))
	{
		philo_eat(philo);
		if (get_stop(philo->prog))
			break ;
		print_status(philo, "is sleeping");
		precise_sleep(philo->prog->time_to_sleep, philo->prog);
		if (get_stop(philo->prog))
			break ;
		print_status(philo, "is thinking");
		usleep(200);
	}
	return (NULL);
}
