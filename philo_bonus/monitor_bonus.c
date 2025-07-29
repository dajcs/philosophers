/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 18:06:24 by anemet            #+#    #+#             */
/*   Updated: 2025/07/30 00:50:15 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

/* Per-child watchdog thread
   Each child keeps a local last_meal timestamp updated at the moment it starts
   eating. The watchdog polls and exits the philo *process* with code 1 when
   philo is starved to death. Parent detects the exit(1) and kills the rest. */
void	*watchdog(void *arg)
{
	t_philo	*ph;
	long long last_meal_time;
	int is_done;

	ph = (t_philo *)arg;
	while (1)
	{
		sem_wait(ph->meal_lock);
		last_meal_time = ph->last_meal;
		is_done = ph->done;
		sem_post(ph->meal_lock);
		if (is_done)
			break;
		if (get_time() - last_meal_time >= ph->prog->t_die)
		{
			print_death(ph->prog, ph->id);
			sem_close(ph->meal_lock);
			sem_unlink(ph->sem_name);
			exit(1);
		}
		usleep(200);
	}
	return (NULL);
}
