/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 16:42:27 by anemet            #+#    #+#             */
/*   Updated: 2025/07/27 09:33:48 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

struct	s_program;

typedef struct s_philo
{
	// --- Static Info ---
	int					id; // the philo's unique number (from 1..N)
	// --- Dynamic State (Protected by meal_lock) ---
	int					eat_count; // How many times this philo has eaten
	long long			last_meal_time; // timestamp of his last meal
	// --- Thread resources link ---
	pthread_t			thread; // the id for this philo's thread
	pthread_mutex_t		*left_fork; // pointer to the fork on the left
	pthread_mutex_t		*right_fork; // pointer to the fork on the right
	struct s_program	*prog; // a pointer back to the shared dashboard
}						t_philo;

// the Shared Dashboard
typedef struct s_program
{
	// --- Simulation rules (Read-only for philos) ---
	int					num_philos; // The total number of philosophers
	int					num_must_eat; // The number of meals
										// for the simulation to stop
	long long			time_to_die; // time a philo survives without eating
	long long			time_to_eat; // time it takes to eat
	long long			time_to_sleep; // time it takes to sleep
	long long			start_time; // the timestamp when the simulation began
	// --- Simulation State & control (Shared and Modified) ---
	int					stop_simulation; // a flag to tell all threads to stop
	// --- Shared Tools (Mutexes) ---
	pthread_mutex_t		write_lock; // lock for printing to the console
	// --- Pointers to Dynamic Data ---
	t_philo				*philos; // an array of all philosophers
	pthread_mutex_t		*forks; // an array of mutexes representing all forks
}						t_program;

/* init.c */
int						init_program(t_program *prog, int argc, char **argv);

/* routine.c */
void					*philosopher_routine(void *argv);

/* monitor.c */
void					*monitor_routine(void *arg);

/* utils.c */
long long				get_time(void);
int						ft_atoi(const char *str);
void					print_status(t_philo *philo, char *status);
void					precise_sleep(long long ms, t_program *prog);

#endif
