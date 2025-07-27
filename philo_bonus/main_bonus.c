/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 15:45:42 by anemet            #+#    #+#             */
/*   Updated: 2025/07/27 16:29:41 by anemet           ###   ########.fr       */
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

/* Wait for all children to finish (one way or another) and cleanup */
static void	wait_for_any_and_cleanup(t_prog *p)
{
	int		i;
	int		status;
	pid_t	pid;

	i = 0;
	while (i < p->n)
	{
		pid = waitpid(-1, &status, 0);
		if (pid > 0 && WIFEXITED(status) && WEXITSTATUS(status) == 1)
			kill_all(p);
		i++;
	}
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
			philo_process(p, i + 1);
			exit(0);
		}
		p->pids[i] = pid;
		i++;
	}
	return (0);
}

int	main(int argc, char **argv)
{
	t_prog		p;
	pthread_t	collector;

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
	wait_for_any_and_cleanup(&p);
	if (p.must_eat != -1)
		pthread_join(collector, NULL);
	return (0);
}
