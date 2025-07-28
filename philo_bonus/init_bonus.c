/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 17:25:39 by anemet            #+#    #+#             */
/*   Updated: 2025/07/28 11:23:31 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

static int	set_defaults(t_prog *p)
{
	p->pids = malloc(sizeof(pid_t) * p->n);
	if (!p->pids)
		return (1);
	return (0);
}

/* Accepts: n t_die t_eat t_sleep [must_eat]; all positive integers */
int	parse_args(t_prog *p, int argc, char **argv)
{
	if (argc < 5 || argc > 6)
		return (1);
	p->n = ft_atoi(argv[1]);
	p->t_die = ft_atoi(argv[2]);
	p->t_eat = ft_atoi(argv[3]);
	p->t_sleep = ft_atoi(argv[4]);
	if (p->n <= 0 || p->t_die <= 0 || p->t_eat <= 0 || p->t_sleep <= 0)
		return (1);
	if (argc == 6)
	{
		p->must_eat = ft_atoi(argv[5]);
		if (p->must_eat <= 0)
			return (1);
	}
	else
		p->must_eat = -1;
	return (set_defaults(p));
}

/* Remove any stale name semaphores from prior runs */
void	close_unlink_sems(void)
{
	sem_unlink(SEM_FORKS);
	sem_unlink(SEM_PRINT);
	sem_unlink(SEM_LIMIT);
	sem_unlink(SEM_MEALS);
}

/* Create named semaphores:
   forks = N, print = 1, limit = N / 2 ("strong" waiter), meals = 0
   Edge case: N==1 -> limit = 1, so the sole philosopher gets a fork */
int	open_sems(t_prog *p)
{
	int	lim;

	close_unlink_sems();
	p->forks = sem_open(SEM_FORKS, O_CREAT, 0644, p->n);
	p->print = sem_open(SEM_PRINT, O_CREAT, 0644, 1);
	lim = p->n / 2;
	if (lim < 1)
		lim = 1;
	p->limit = sem_open(SEM_LIMIT, O_CREAT, 0644, lim);
	p->meals = sem_open(SEM_MEALS, O_CREAT, 0644, 0);
	if (p->forks == SEM_FAILED || p->print == SEM_FAILED
		|| p->limit == SEM_FAILED || p->meals == SEM_FAILED)
		return (1);
	return (0);
}
