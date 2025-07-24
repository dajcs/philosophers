/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 17:13:22 by anemet            #+#    #+#             */
/*   Updated: 2025/07/24 13:44:11 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	destroy_all(t_program *prog)
{
	int	i;

	i = 0;
	while (i < prog->num_philos)
	{
		pthread_mutex_destroy(&prog->forks[i]);
		pthread_mutex_destroy(&prog->philos[i].meal_lock);
		i++;
	}
	pthread_mutex_destroy(&prog->write_lock);
	pthread_mutex_destroy(&prog->stop_lock);
	free(prog->philos);
	free(prog->forks);
}

// 1. set the "Zero Hour"
// 2. Launch all Philosopher threads (pthread_create)
// 3. launch the Monitor thread
// 4. wait for the simulation to end (pthread_join)
static int	start_simulation(t_program *prog)
{
	int			i;
	pthread_t	monitor;

	i = 0;
	prog->start_time = get_time();
	while (i < prog->num_philos)
	{
		prog->philos[i].last_meal_time = prog->start_time;
		if (pthread_create(&prog->philos[i].thread, NULL, &philosopher_routine,
				&prog->philos[i]) != 0)
			return (1);
		i++;
	}
	if (pthread_create(&monitor, NULL, &monitor_routine, prog) != 0)
		return (1);
	i = 0;
	while (i < prog->num_philos)
	{
		pthread_join(prog->philos[i].thread, NULL);
		i++;
	}
	pthread_join(monitor, NULL);
	return (0);
}

int	main(int argc, char **argv)
{
	t_program	prog;

	if (argc < 5 || argc > 6)
	{
		printf("Usage: %s n_philos t_die t_eat t_sleep [n_meals]\n", argv[0]);
		return (1);
	}
	if (init_program(&prog, argc, argv) != 0)
	{
		printf("Error: invalid arguments or initialization failed\n");
		return (1);
	}
	if (start_simulation(&prog) != 0)
	{
		printf("Error: Failed to create threads.\n");
		destroy_all(&prog);
		return (1);
	}
	destroy_all(&prog);
	return (0);
}
