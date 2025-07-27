/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 17:48:15 by anemet            #+#    #+#             */
/*   Updated: 2025/07/27 18:04:47 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

/* Release both forks and leave the acquisition region */
static void	put_two_forks(t_philo *ph)
{
	sem_post(ph->prog->forks);
	sem_post(ph->prog->forks);
	sem_post(ph->prog->limit);
}

/* Enter "critical acquisition region" (SEM_LIMIT) then acquire two forks */
static void	take_two_forks(t_philo *ph)
{
	sem_wait(ph->prog->limit);
	sem_wait(ph->prog->forks);
	print_status(ph->prog, ph->id, "has taken a fork");
	sem_wait(ph->prog->forks);
	print_status(ph->prog, ph->id, "has taken a fork");
}

/* One eat cycle: grab 2 forks, mark last_meal, eat t_eat, release forks */
static void	eat_block(t_philo *ph)
{
	take_two_forks(ph);
	ph->last_meal = get_time();
	print_status(ph->prog, ph->id, "is eating");
	ph->eat_count = ph->eat_count + 1;
	precise_sleep(ph->prog->t_eat);
	put_two_forks(ph);
}

/* If must_eat is set and reached, notify parent (SEM_MEALS) and exit soon */
static int	check_meals_and_exit(t_philo *ph)
{
	if (ph->prog->must_eat == -1)
		return (0);
	if (ph->eat_count >= ph->prog->must_eat)
	{
		sem_post(ph->prog->meals);
		return (1);
	}
	return (0);
}

void	philo_process(t_prog *p, int id)
{
	t_philo	ph;

	ph.id = id;
	ph.eat_count = 0;
	ph.prog = p;
	ph.last_meal = p->start_time;
	pthread_create(&ph.monitor, NULL, &watchdog, &ph);
	if (id % 2 == 0)
		usleep(5000);
	while (1)
	{
		eat_block(&ph);
		if (check_meals_and_exit(&ph))
			exit(0);
		print_status(p, id, "is sleeping");
		precise_sleep(p->t_sleep);
		print_status(p, id, "is thinking");
	}
}
