/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 17:48:15 by anemet            #+#    #+#             */
/*   Updated: 2025/07/30 17:03:34 by anemet           ###   ########.fr       */
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
	sem_wait(ph->meal_lock);
	ph->last_meal = get_time();
	sem_post(ph->meal_lock);
	print_status(ph->prog, ph->id, "is eating");
	ph->eat_count++;
	precise_sleep(ph->prog->t_eat);
	put_two_forks(ph);
}

/* If must_eat is set and reached, notify parent (SEM_MEALS) and exit soon */
// `sem_close` to make valgrind happy. This is closing the handle only
// so the semaphor is available to other processes until a `sem_unlink`
static int	check_meals_and_exit(t_philo *ph)
{
	if (ph->prog->must_eat == -1)
		return (0);
	if (ph->eat_count >= ph->prog->must_eat)
	{
		sem_wait(ph->meal_lock);
		ph->done = 1;
		sem_post(ph->meal_lock);
		sem_post(ph->prog->meals);
		return (1);
	}
	return (0);
}

// the main philo process
// set ph struct initial values
// create watchdog detecting starvation to death
// detach watchdog thread because of valgrind
// every second philo should wait +5 ms to let the others grab 2 forks and eat
// when nr philosophers is odd, a mandatory 0.2 ms thinking is prescribed --
// this will allow other hungry ph to get in the restaurant
void	philo_process(t_prog *p, int id)
{
	t_philo	ph;

	if (!set_philo(&ph, p, id))
		exit(1);
	pthread_create(&ph.monitor, NULL, &watchdog, &ph);
	if (id % 2 == 0)
		usleep(5000);
	while (1)
	{
		eat_block(&ph);
		if (check_meals_and_exit(&ph))
			break ;
		print_status(p, id, "is sleeping");
		precise_sleep(p->t_sleep);
		print_status(p, id, "is thinking");
		if (p->n % 2 == 1)
			usleep(200);
	}
	pthread_join(ph.monitor, NULL);
	sem_close(ph.meal_lock);
	sem_unlink(ph.sem_name);
	exit(0);
}
