/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 18:06:24 by anemet            #+#    #+#             */
/*   Updated: 2025/07/30 17:08:36 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

/* Per-child watchdog thread
   Each child keeps a local last_meal timestamp updated at the moment it starts
   eating. The watchdog polls and exits the philo *process* with code 1 when
   philo is starved to death. Parent detects the exit(1) and kills the rest. */
void	*watchdog(void *arg)
{
	t_philo		*ph;
	long long	last_meal_time;
	int			is_done;

	ph = (t_philo *)arg;
	while (1)
	{
		sem_wait(ph->meal_lock);
		last_meal_time = ph->last_meal;
		is_done = ph->done;
		sem_post(ph->meal_lock);
		if (is_done)
			break ;
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

// builds a string like "/ph_lock_123" from a base and an id up to 999
static void	build_sem_name(char *buf, const char *base, int id)
{
	char	*p;

	p = buf;
	while (*base)
		*p++ = *base++;
	*p++ = (id / 100) + '0';
	*p++ = ((id % 100) / 10) + '0';
	*p++ = (id % 10) + '0';
	*p = '\0';
}

// set t_philo struct with initial values
int	set_philo(t_philo *ph, t_prog *p, int id)
{
	ph->id = id;
	ph->eat_count = 0;
	ph->prog = p;
	ph->done = 0;
	ph->last_meal = p->start_time;
	build_sem_name(ph->sem_name, "/ph_lock_", id);
	sem_unlink(ph->sem_name);
	ph->meal_lock = sem_open(ph->sem_name, O_CREAT, 0644, 1);
	if (ph->meal_lock == SEM_FAILED)
		return (0);
	return (1);
}
