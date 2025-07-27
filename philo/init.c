/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 12:15:20 by anemet            #+#    #+#             */
/*   Updated: 2025/07/27 13:11:38 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

// dynamic malloc for forks mutexes
// initiate prog->forks[i] mutexes (protect forks while eating)
// initiate prog->write_lock mutex (protect terminal writing by philos/monitor)
// initiate prog->stop_lock mutex (protect end-of-game)
static int	init_mutexes(t_program *prog)
{
	int	i;

	i = 0;
	prog->forks = malloc(sizeof(pthread_mutex_t) * prog->num_philos);
	if (!prog->forks)
		return (1);
	while (i < prog->num_philos)
	{
		if (pthread_mutex_init(&prog->forks[i], NULL) != 0)
			return (1);
		i++;
	}
	if (pthread_mutex_init(&prog->write_lock, NULL) != 0)
		return (1);
	return (0);
}

static void	init_philos(t_program *prog)
{
	int	i;

	i = 0;
	while (i < prog->num_philos)
	{
		prog->philos[i].id = i + 1;
		prog->philos[i].eat_count = 0;
		prog->philos[i].prog = prog;
		prog->philos[i].left_fork = &prog->forks[i];
		if (i == 0)
			prog->philos[i].right_fork = &prog->forks[prog->num_philos - 1];
		else
			prog->philos[i].right_fork = &prog->forks[i - 1];
		i++;
	}
}

static int	parse_args(t_program *prog, int argc, char **argv)
{
	prog->num_philos = ft_atoi(argv[1]);
	prog->time_to_die = ft_atoi(argv[2]);
	prog->time_to_eat = ft_atoi(argv[3]);
	prog->time_to_sleep = ft_atoi(argv[4]);
	if (prog->num_philos <= 0 || prog->time_to_die <= 0
		|| prog->time_to_eat <= 0 || prog->time_to_sleep <= 0)
		return (1);
	if (argc == 6)
	{
		prog->num_must_eat = ft_atoi(argv[5]);
		if (prog->num_must_eat <= 0)
			return (1);
	}
	else
		prog->num_must_eat = -1;
	return (0);
}

int	init_program(t_program *prog, int argc, char **argv)
{
	if (parse_args(prog, argc, argv) != 0)
		return (1);
	prog->philos = malloc(sizeof(t_philo) * prog->num_philos);
	if (!prog->philos)
		return (1);
	if (init_mutexes(prog) != 0)
	{
		free(prog->philos);
		return (1);
	}
	init_philos(prog);
	prog->stop_simulation = 0;
	return (0);
}
