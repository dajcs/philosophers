/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 15:45:42 by anemet            #+#    #+#             */
/*   Updated: 2025/07/31 10:00:43 by anemet           ###   ########.fr       */
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
static void	reap_children(t_prog *p, pthread_t collector)
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
		if (pid > 0 && WIFEXITED(status) && WEXITSTATUS(status) == 1 && !died)
		{
			kill_all(p);
			died = 1;
		}
		i++;
	}
	if (p->must_eat != -1 && died == 1)
		pthread_cancel(collector);
}

/* Collector waits until every philosopher posted once to SEM_MEALS */
/*
3. meals_collector (Thread inside Parent Process, if quota)
 └─ (loop N times)
     ├─ sem_wait (meals)
     └─ if (all have eaten)
         ├─ sem_wait (print)
         └─ kill_all
             └─ kill*/
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
	sem_wait(p->print);
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
main (Parent Process)
 ├─ parse_args
 │   └─ ft_atoi
 ├─ open_sems
 │   ├─ close_unlink_sems
 │   │   └─ sem_unlink
 │   └─ sem_open
 ├─ get_time
 ├─ spawn_all
 │   └─ fork (for each philosopher) -> Creates a child `philo_process`
 ├─ pthread_create (if quota) -> meals_collector (thread in parent)
 ├─ reap_children
 │   ├─ waitpid (loops for all children)
 │   └─ kill_all (if a child exits with status 1)
 │       └─ kill
 └─ Cleanup
     ├─ pthread_join (for meals_collector)
     ├─ close_sems
     │   └─ sem_close
     └─ close_unlink_sems
         └─ sem_unlink
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
	reap_children(&p, collector);
	if (p.must_eat != -1)
		pthread_join(collector, NULL);
	close_sems(&p);
	free(p.pids);
	close_unlink_sems();
	return (0);
}
