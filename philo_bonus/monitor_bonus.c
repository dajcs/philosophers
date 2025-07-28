/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 18:06:24 by anemet            #+#    #+#             */
/*   Updated: 2025/07/28 16:14:54 by anemet           ###   ########.fr       */
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

	ph = (t_philo *)arg;
	while (1)
	{
		if (get_time() - ph->last_meal > ph->prog->t_die)
		{
			print_death(ph->prog, ph->id);
			exit(1);
		}
		usleep(200);
	}
	return (NULL);
}
