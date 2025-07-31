/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 13:53:01 by anemet            #+#    #+#             */
/*   Updated: 2025/07/31 09:42:33 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	get_stop(t_program *p)
{
	int	val;

	pthread_mutex_lock(&p->stop_lock);
	val = p->stop_simulation;
	pthread_mutex_unlock(&p->stop_lock);
	return (val);
}

void	set_stop(t_program *p, int v)
{
	pthread_mutex_lock(&p->stop_lock);
	p->stop_simulation = v;
	pthread_mutex_unlock(&p->stop_lock);
}

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
		set_stop(prog, 1);
		return (1);
	}
	return (0);
}

static int	check_if_dead(t_philo *philo)
{
	long long	time_since_meal;

	pthread_mutex_lock(&philo->meal_lock);
	time_since_meal = get_time() - philo->last_meal_time;
	pthread_mutex_unlock(&philo->meal_lock);
	if (time_since_meal > philo->prog->time_to_die)
	{
		set_stop(philo->prog, 1);
		print_status(philo, "died");
		return (1);
	}
	return (0);
}

/*
2. monitor_routine (one central thread)
 └─ (infinite loop)
     ├─ check_if_dead (for each philosopher)
     │   ├─ pthread_mutex_lock (meal_lock)
     │   ├─ get_time
     │   ├─ pthread_mutex_unlock (meal_lock)
     │   ├─ set_stop (if dead)
     │   │   ├─ pthread_mutex_lock (stop_lock)
     │   │   └─ pthread_mutex_unlock (stop_lock)
     │   └─ print_status ("died")
     │       ├─ pthread_mutex_lock (write_lock)
     │       ├─ get_stop
     │       └─ pthread_mutex_unlock (write_lock)
     └─ check_if_all_ate (if num_must_eat is set)
         ├─ pthread_mutex_lock (meal_lock)
         ├─ pthread_mutex_unlock (meal_lock)
         └─ set_stop (if all ate)
             ├─ pthread_mutex_lock (stop_lock)
             └─ pthread_mutex_unlock (stop_lock)
*/
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
		usleep(200);
	}
	return (NULL);
}
