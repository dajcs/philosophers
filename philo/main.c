/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 17:13:22 by anemet            #+#    #+#             */
/*   Updated: 2025/07/26 17:19:17 by anemet           ###   ########.fr       */
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
//      before launch set `last_meal_time` to prevent instant death by hunger
/* ** pthread_create() arguments:
    *   `&prog->philos[i].thread`: Store the ID of the thread at the given
	                               pointer variable (of type pthread_t)
    *   `NULL`: thread attributes, `NULL` -> Default thread attributes.
    *   `&philosopher_routine`: pointer to the function the new thread should
	*                           start running.
	                            this function is of type `void *(*)(void *)`
                       meaning: takes 1 arg - a generic pointer to variable
                       and returns result in the form of a generic pointer ->
                                          retrieved later with `pthread_join()`
    *   `&prog->philos[i]`: start `philosopher_routine()` with this argument
                                     (a pointer to the philosopher's profile)
         so the `philosopher_routine()` has access to the struct type `t_philo`
   ** The Result:** From the moment `pthread_create()` is called, the
      `philosopher_routine` function begins running **concurrently** with the
	  `start_simulation` function. The `main` thread continues its loop to
	  launch the next philosopher, while the first philosopher is already
	  starting their eat/sleep/think cycle.
*/
// 3. launch the Monitor thread with arguments:
/*    - &monitor: ptr to variable monitor to store thread id (type pthread_t)
      - NULL    : default thread attributes
	  - &monitor_routine: ptr to the function to be run in this thread
	  - prog    : ptr to the "Shared Dashboard" struct (type: t_program)
*/
/*  after this step we have:
	..- `number_of_philosophers` * `philosopher_routine()` thread
	  - and                    1 * `monitor_routine()` thread
	running inside the parent thread (`start_simulation()`)
	*/
// 4. wait for the simulation to end (pthread_join)
//         (when a philo dies or when all have eaten `num_must_eat` times)
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
