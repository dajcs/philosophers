/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 13:53:01 by anemet            #+#    #+#             */
/*   Updated: 2025/07/27 13:12:37 by anemet           ###   ########.fr       */
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
		if (prog->philos[i].eat_count >= prog->num_must_eat)
			full_philos++;
		i++;
	}
	if (full_philos == prog->num_philos)
	{
		prog->stop_simulation = 1;
		return (1);
	}
	return (0);
}

static int	check_if_dead(t_philo *philo)
{
	long long	time;

	time = get_time();
	if ((time - philo->last_meal_time) > philo->prog->time_to_die)
	{
		philo->prog->stop_simulation = 1;
		print_status(philo, "died");
		return (1);
	}
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
		usleep(1000);
	}
	return (NULL);
}
