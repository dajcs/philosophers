

## Philosophers problem overview

- One or more philosphers sit at a round table. \
  There is a large bowl of spaghetti in the middle of the table.
- The philosophers take turns eating, thinking, and sleeping. \
  While they are eating, they are not thinking nor sleeping; \
  while thinking, they are not eating nor sleeping;
  and, of course, while sleeping, they are not eating nor thinking.
- There are forks on the table. There are **as many forks as philosophers**.
- Since eating spaghetti with just one fork is impractical, a philosopher must pick up both the fork to their right and the fork to their left before eating.
- When a philosopher has finished eating, they put their forks back on the table and start sleeping. Once awake, they start thinking again. The simulation stops when a philosopher dies of starvation.
- Every philosopher needs to eat and should never starve.
- Philosphers do not communicate with each ohter.
- Philosophers do not know if another philosopher is about to die.
- Philosophers should avoid dying!

## Global rules

- global variables are forbidden
- the program takes the following arguments:
  - `number_of_philosophers`: this is the number of philosophers and also the number of forks
  - `time_to_die` (miliseconds): If a philosopher has not started eating within `time_to_die` miliseconds since the start of their last meal or the start of the simulation, they die
  - `time_to_eat` (miliseconds): The time it takes for a philosopher to eat. During that time, they will need to hold two forks.
  - `time_to_sleep` (miliseconds): The time a philosopher will spend sleeping.
  - `number_of_times_each_philosopher_must_eat` (optional argument): If all philosophers have eaten at least this many times, the simulation stops. If not specified, the simulation stops when a philosopher dies.
- Each philosopher has a number ranging from 1 to `number_of_philosophers`.
- Philosopher number `1` sits next to philosopher number `number_of_philosophers`. Any other philosopher, numbered `N`, sits between philosopher `N-1` and philosopher `N+1`.
- Any state change of a philosopher must be formatted as follows:
  - timestamp_in_ms X has taken a fork
  - timestamp_in_ms X is eating
  - timestamp_in_ms X is sleeping
  - timestamp_in_ms X is thinking
  - timestamp_in_ms X died

  Replace timestamp_in_ms with the current timestamp in milliseconds and X with the philosopher number.
- A displayed state message should not overlap with another message
- a message announcing a philosopher's death must be displayed within 10 ms of their actual death
- but again, philosophers should avoid dying!
- the program must not have any data races

## Mandatory part

- program name: `philo`
- files: Makefile, *.h, *.c in directory `philo`
- Makefile: NAME, all, clean, fclean, re
- Arguments: number_of_philosophers time_to_die time_to_eat time_to_sleep \[number_of_times_each_philosopher_must_eat\]
- Description: Philosophers with threads and mutexes

The specific rules for the mandatory part are:
- Each philosopher must be represented as a separate thread
- There is one fork between each pair of philosophers. Therefore, if there are several philosophers, each philosopher has a fork in their left side and a fork on their right side. If there is only one philosopher, they will have access to just one fork.
- To prevent philosophers from duplicating forks, it should be protected each fork's state with mutex.

## Constraints

- allowed external functions: `memset`, `printf`, `malloc`, `free`, `write`, `usleep`, `gettimeofday`, `pthread_create`, `pthread_detach`, `pthread_join`, `pthread_mutex_init`, `pthread_mutex_destroy`, `pthread_mutex_lock`, `pthread_mutex_unlock`
- the **Norminette rules** should be adhered:
  - only while loops are allowed
  - ternary operators are not allowed
  - variable definition and variable assignment in separate rows
  - one empty line after variable definitions, no other empty lines
  - only one variable definition/assignment or instruction on a single line
  - max 80 characters / line
  - max 4 parameters / function
  - max 5 variables / function
  - max 25 lines / function
  - max 5 functions / .c file
