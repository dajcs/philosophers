# philosophers
[philosophers project](.test/philosophers.pdf) @ school 42

- CodeVault: [Intro to threads, processes, mutexes & semaphores](https://www.youtube.com/watch?v=d9s_d28yJq0&list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2&index=1)

The bonus part is terminated by killing all processes (when eat-quota reached or when a philo dies) -- this makes valgrind complain, since memory reservations are not released. In reality the OS is releasing all the memory allocated by these processes, so there is no memory leak, but interpretation of valgrind messages depends of the evaluator.
Graceful exit of the processes could have been done with `sem_trywait` (checking of a stop semaphore) but unfortunately this is not included in the allowed external functions.
