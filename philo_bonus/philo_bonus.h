/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 14:26:29 by anemet            #+#    #+#             */
/*   Updated: 2025/07/27 15:24:13 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */
/* @brief: Dining Philosophers problem using processes + semaphores

   Semaphores:
		- SEM_FORKS: counting (N). Models the pool of forks; each meal consumes 2
		- SEM_LIMIT: counting (N-1). "Waiter" throtttle - prevents deadlock,
					at most N-1 philosophers try to pick up forks simultaneously
		- SEM_PRINT: binary (1). Serializes stdout so messages never interleave.
		- SEM_MEALS: counting (0). Parent waits for N posts when each child
					its required meal count (when optional argument provided)

   Processes:
	- Parent: spawns N child processes, tracks termination, kills all on death,
				or stops when everyone ate enough.
	- Child:  one / philosopher, runs the eat/sleep/think loop. Each child
				launches an in-process watchdog thread to detect its own death

 * ************************************************************************* */

#ifndef PHILO_BONUS_H
# define PHILO_BONUS_H

# include <fcntl.h> // file control
# include <pthread.h> // pthread_create/join (monitor thread)
# include <semaphore.h> // sem_open, sem_close
# include <signal.h> // kill
# include <stdio.h> // printf
# include <stdlib.h> // malloc, free, exit
# include <string.h> // strcmp
# include <sys/time.h> // gettimeofday
# include <sys/types.h> // types
# include <sys/wait.h> // waitpid
# include <unistd.h> // fork, usleep

# define SEM_FORKS "/ph_forks"
# define SEM_PRINT "/ph_print"
# define SEM_LIMIT "/ph_limit"
# define SEM_MEALS "/ph_meals"

typedef struct s_prog
{
	int			n; // number of philosophers (== number of forks)
	int			must_eat; // target meals per philosopher, -1 if none
	long long	t_die; // [ms] max time without eating / starve to death
	long long	t_eat; // [ms] time needed to eat (holding 2 forks)
	long long	t_sleep; // [ms] time must sleep
	long long	start_time; // simulation epoch for timestamps
	/* Named semaphores -- shared by all processes */
	sem_t		*forks;
	sem_t		*print;
	sem_t		*limit;
	sem_t		*meals;

	pid_t		*pids; // child PIDs for mass-kill on termination
}				t_prog;

typedef struct s_philo
{
	int			id; // philo id, 1..N
	int			eat_count; // meals eaten so far
	long long	last_meal; // timestamp start of last meal
	t_prog		*prog; // back-pointer to shared program data
	pthread_t	monitor; // watchdog thread (in the child process)
}				t_philo;

/* main_bonus.c */
void			kill_all(t_prog *p);
int				spawn_all(t_prog *p);

/* init_bonus.c */
int				parse_args(t_prog *p, int argc, char **argv);
int				open_sems(t_prog *p);
void			close_unlink_sems(void);

/* routine_bonus.c */
void			philo_process(t_prog *p, int id);

/* monitor_bonus.c */
void			*watchdog(void *arg);

/* utils_bonus.c */
long long		get_time(void);
int				ft_atoi(const char *str);
void			precise_sleep(long long ms);
void			print_status(t_prog *p, int id, char *status);

#endif
