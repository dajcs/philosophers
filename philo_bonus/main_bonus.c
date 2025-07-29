/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 15:45:42 by anemet            #+#    #+#             */
/*   Updated: 2025/07/29 15:16:23 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

/* Send SIGTERM then SIGKILL to ensure all children are gone */
void	kill_all(t_prog *p)
{
	int	i;

	i = 0;
	while (i < p->n)
	{
		if (p->pids[i] > 0)
			kill(p->pids[i], SIGTERM);
		i++;
	}
	i = 0;
	while (i < p->n)
	{
		if (p->pids[i] > 0)
			kill(p->pids[i], SIGKILL);
		i++;
	}
}

/* Wait for all children to finish (when quota specified) and cleanup */
// if a philo dies of hunger (exit (1)) => kill all philos alive
// if a philo dies and eating quota was specified
//
static void	wait_for_any_and_cleanup(t_prog *p, pthread_t collector)
{
	int		i;
	int		status;
	pid_t	pid;
	int		died;

	i = 0;
	died = 0;
	while (i < p->n)
	{
		pid = waitpid(-1, &status, 0);
		if (pid > 0 && WIFEXITED(status) && WEXITSTATUS(status) == 1)
		{
			kill_all(p);
			died = 1;
		}
		i++;
	}
	if (p->must_eat != -1 && died == 1)
		pthread_cancel(collector);
	close_unlink_sems();
	free(p->pids);
}

/* Collector waits until every philosopher posted once to SEM_MEALS */
static void	*meals_collector(void *arg)
{
	t_prog	*p;
	int		i;

	p = (t_prog *)arg;
	i = 0;
	while (i < p->n)
	{
		sem_wait(p->meals);
		i++;
	}
	kill_all(p);
	return (NULL);
}

/* Fork N children; each child enters philo_process and never returns */
// inside child (pid == 0)
//     free(p->pids); p->pids = NULL because of valgrind
int	spawn_all(t_prog *p)
{
	int		i;
	pid_t	pid;

	i = 0;
	while (i < p->n)
	{
		pid = fork();
		if (pid < 0)
			return (1);
		if (pid == 0)
		{
			free(p->pids);
			p->pids = NULL;
			philo_process(p, i + 1);
			_exit(0);
		}
		p->pids[i] = pid;
		i++;
	}
	return (0);
}

/* *******************    Function Call Map   ******************************
main
 ├─ parse_args
 ├─ open_sems
 ├─ spawn_all
 │    └─ fork -> child: philo_process
 │                     ├─ pthread_create (watchdog)
 │                     ├─ eat_block
 │                     │    ├─ take_two_forks
 │                     │    └─ put_two_forks
 │                     ├─ (if quota) sem_post(SEM_MEALS) + exit(0)
 │                     └─ (loop)
 ├─ (if quota) pthread_create (meals_collector)
 │             └─ meals_collector
 │                 ├─ loop N×: sem_wait(SEM_MEALS)
 │                 └─ kill_all
 └─ wait_for_any_and_cleanup
      ├─ loop N×: waitpid(-1)
      ├─ (if any exit(1)) kill_all
      ├─ (if quota && died) cancel `meals_collector`
      └─ close_unlink_sems + free
*/
int	main(int argc, char **argv)
{
	t_prog		p;
	pthread_t	collector;

	collector = 0;
	if (parse_args(&p, argc, argv) != 0)
		return (printf("Usage: %s n t_die t_eat t_sleep [n_meals]\n", argv[0]),
			1);
	if (open_sems(&p) != 0)
		return (printf("Error: sem_open\n"), 1);
	p.start_time = get_time();
	if (spawn_all(&p) != 0)
		return (printf("Error: fork\n"), 1);
	if (p.must_eat != -1)
		pthread_create(&collector, NULL, &meals_collector, &p);
	wait_for_any_and_cleanup(&p, collector);
	if (p.must_eat != -1)
		pthread_join(collector, NULL);
	close_sems(&p);
	close_unlink_sems();
	return (0);
}
