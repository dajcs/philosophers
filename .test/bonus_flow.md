# Case 1 — must_eat == -1 (no quota, stop on first death)



```arduino
PARENT (main)                                   CHILD (philo i)                   CHILD's watchdog thread          PARENT collector thread
-----------------                               -----------------                 -----------------------          -----------------------
main()
  ├─ parse_args()
  ├─ open_sems()
  ├─ p.start_time = get_time()
  ├─ spawn_all()  ─────────────────────────────►  fork() == 0 ? philo_process(i)
  ├─ pthread_create(&collector, meals_collector, &p)                               (created earlier)
  └─ wait_for_any_and_cleanup()                 philo_process(p, i)
        (reap children)                           ├─ pthread_create(monitor, watchdog, &ph)
                                                  └─ while (1) {
                                                       eat_block(&ph)              watchdog(&ph)
                                                       if (eat_count >= must_eat)     loop, but no death
                                                         sem_post(SEM_MEALS)          (child keeps itself alive)
                                                         exit(0)  // child done
                                                       print "is sleeping"
                                                       precise_sleep(t_sleep)
                                                       print "is thinking"
                                                     }

meals_collector(&p)   // runs concurrently in PARENT
  └─ for i in [1..N]: sem_wait(SEM_MEALS)  ◄───── each child posts once on quota
  └─ kill_all(p)  // idempotent stop signal to any stragglers

wait_for_any_and_cleanup()
  ├─ reap all children (mostly exit(0))
  └─ close_unlink_sems(), free(pids), return

main()
  ├─ pthread_join(collector, NULL)
  └─ return (program terminates)
```

### Example:
```yaml
./philo_bonus 2 200 200 200
0 1 has taken a fork
0 1 has taken a fork
0 1 is eating
200 1 is sleeping
200 2 has taken a fork
200 2 has taken a fork
200 2 is eating
201 1 died
```

```yaml
0 1 has taken a fork
```
- **child P1**: `philo_process()` → `eat_block()` → `take_two_forks()` → `print_status("has taken a fork")`

```yaml
0 1 has taken a fork
```
- **child P1**: `philo_process()` → `eat_block()` → `take_two_forks()` → `print_status("has taken a fork")` (second fork)

```yaml
0 1 is eating
```
- **child P1**: `philo_process()` → `eat_block()` → (sets `last_meal`) → `print_status("is eating")`

```yaml
200 1 is sleeping
```
- **child P1**: `philo_process()` → (after `eat_block()` + `put_two_forks()`) → `print_status("is sleeping")`

...

```yaml
201 1 died
```
- **monitor P1**: `watchdog()` → condition `(now - last_meal > t_die)` true → `print_status("died")` → `exit(1)`





```arduino
PARENT (main)                      CHILD P1 (philo_process)           P1 WATCHDOG (watchdog)      CHILD P2 (philo_process)
----------------                   ---------------------------         -----------------------      ---------------------------
main()
  ├─ parse_args()
  ├─ open_sems()
  ├─ p.start_time = get_time()
  ├─ spawn_all() -----------------> fork -> philo_process(p,1)
  │                                 ├─ pthread_create(monitor,watchdog,&ph1)
  │                                 └─ loop: eat_block(); sleep; think
  │
  ├─ spawn_all() -----------------> fork -> philo_process(p,2)
  │                                 ├─ pthread_create(monitor,watchdog,&ph2)
  │                                 └─ loop: eat_block(); sleep; think
  │
  └─ wait_for_any_and_cleanup()
       ├─ waitpid(-1,&st,0) <----- exit(1) @201ms  (watchdog prints "201 1 died"; exit(1))
       │    └─ if (WEXITSTATUS==1) -> kill_all(p)
       ├─ (reap remaining children killed by signals)
       └─ close_unlink_sems(); free(pids); return

Notes ↴
- Death trigger: watchdog() in P1 sees (now - last_meal > t_die) and does
  print_status(...,"died") + exit(1).
- Parent sees exit(1) inside wait_for_any_and_cleanup() and calls kill_all().
```


# Case 2 — must_eat specified (stop when everyone reached quota, or earlier on death)


## 2.1 — Normal “all reach quota” path

```arduino
PARENT (main)                                   CHILD (philo i)                   CHILD's watchdog thread          PARENT collector thread
-----------------                               -----------------                 -----------------------          -----------------------
main()
  ├─ parse_args()
  ├─ open_sems()
  ├─ p.start_time = get_time()
  ├─ spawn_all()  ─────────────────────────────►  fork() == 0 ? philo_process(i)
  ├─ pthread_create(&collector, meals_collector, &p)                               (created earlier)
  └─ wait_for_any_and_cleanup()                 philo_process(p, i)
        (reap children)                           ├─ pthread_create(monitor, watchdog, &ph)
                                                  └─ while (1) {
                                                       eat_block(&ph)              watchdog(&ph)
                                                       if (eat_count >= must_eat)     loop, but no death
                                                         sem_post(SEM_MEALS)          (child keeps itself alive)
                                                         exit(0)  // child done
                                                       print "is sleeping"
                                                       precise_sleep(t_sleep)
                                                       print "is thinking"
                                                     }

meals_collector(&p)   // runs concurrently in PARENT
  └─ for i in [1..N]: sem_wait(SEM_MEALS)  ◄───── each child posts once on quota
  └─ kill_all(p)  // idempotent stop signal to any stragglers

wait_for_any_and_cleanup()
  ├─ reap all children (mostly exit(0))
  └─ close_unlink_sems(), free(pids), return

main()
  ├─ pthread_join(collector, NULL)
  └─ return (program terminates)
```


### Example:
```yaml
./philo_bonus 2 410 200 200 2
1 1 has taken a fork
1 1 has taken a fork
1 1 is eating
201 1 is sleeping
201 2 has taken a fork
201 2 has taken a fork
201 2 is eating
401 2 is sleeping
401 1 is thinking
401 1 has taken a fork
401 1 has taken a fork
401 1 is eating
601 2 is thinking
601 2 has taken a fork
601 2 has taken a fork
601 2 is eating
```

```arduino
PARENT (main)                      CHILD P1                          CHILD P2                      COLLECTOR THREAD (meals_collector)
----------------                   ---------------------------       ---------------------------   -----------------------------------
main()
  ├─ parse_args()
  ├─ open_sems()
  ├─ p.start_time = get_time()
  ├─ spawn_all() -----------------> fork -> philo_process(p,1)
  ├─ spawn_all() -----------------> fork -> philo_process(p,2)
  ├─ pthread_create(&collector, meals_collector, &p)
  └─ wait_for_any_and_cleanup()    philo_process(p,1)                philo_process(p,2)
                                     loop:                             loop:
                                       eat_block(); ecount++            eat_block(); ecount++
                                       ...                              ...
                                       if (ecount==must_eat)           if (ecount==must_eat)
                                         sem_post(SEM_MEALS)             sem_post(SEM_MEALS)
                                         exit(0)                         exit(0)

meals_collector(&p)
  ├─ sem_wait(SEM_MEALS)  <------ post by P1
  ├─ sem_wait(SEM_MEALS)  <------ post by P2
  └─ kill_all(p)  // idempotent; OK if kids already exited

wait_for_any_and_cleanup()
  ├─ reap children (mostly exit(0))
  └─ cleanup (unlink sems, free), return
main()
  └─ pthread_join(collector,NULL) and exit
```


## 2.2 — Someone dies before all reach quota (with the “wake the collector” fix)

```arduino
PARENT (main)                                   CHILD (philo j)                   CHILD's watchdog thread          PARENT collector thread
-----------------                               -----------------                 -----------------------          -----------------------
main()
  ├─ parse_args()
  ├─ open_sems()
  ├─ p.start_time = get_time()
  ├─ spawn_all()
  ├─ pthread_create(&collector, meals_collector, &p)
  └─ wait_for_any_and_cleanup()
        ├─ waitpid(-1, &status, 0)  ◄─────────── child j exits(1) (watchdog)
        │     died = 1; kill_all(p);
        ├─ (continue reaping remaining children)      (others die due to signals)
        ├─ if (must_eat != -1 && died == 1)           // FIX: unblock collector
        │     repeat N times: sem_post(SEM_MEALS)  ─────────────────────────────────────────► wakes collector loop
        └─ close_unlink_sems(), free(pids), return

meals_collector(&p)
  └─ for i in [1..N]: sem_wait(SEM_MEALS)   ◄───────── either real posts (quota) or “fake” wake posts (on death)
  └─ kill_all(p)  // idempotent; OK even after parent’s kill_all

main()
  └─ pthread_join(collector, NULL)  // joins cleanly thanks to wake posts
     return (program terminates)

```

### Example:

```yaml
./philo_bonus 2 200 200 200 2
1 1 has taken a fork
1 1 has taken a fork
1 1 is eating
201 1 is sleeping
201 2 has taken a fork
201 2 has taken a fork
201 2 is eating
202 1 died
```

```arduino
PARENT (main)                          CHILD P1                         P1 WATCHDOG              CHILD P2                      COLLECTOR
----------------                       ---------------------------      ----------------         ---------------------------   -------------
main()
  ├─ parse_args()
  ├─ open_sems()
  ├─ p.start_time = get_time()
  ├─ spawn_all() --------------------> philo_process(p,1)
  ├─ spawn_all() --------------------> philo_process(p,2)
  ├─ pthread_create(&collector, meals_collector, &p)
  └─ wait_for_any_and_cleanup()
       ├─ waitpid(-1,&st,0) <------- exit(1) @202ms  (watchdog prints "202 1 died"; exit(1))
       │    ├─ died=1
       │    └─ kill_all(p)
       ├─ (reap remaining children)
       ├─ if (must_eat!=-1 && died)
       │     loop N times: sem_post(SEM_MEALS)  -------------------------------> unblocks collector()
       └─ cleanup + return

philo_process(p,1)
  loop...
  (dies via watchdog)

philo_process(p,2)
  loop...
  (killed by parent's kill_all)

meals_collector(&p)
  ├─ sem_wait(SEM_MEALS)  <------ woken by parent’s “fake” posts (since a death happened)
  ├─ sem_wait(SEM_MEALS)
  └─ kill_all(p)  // idempotent; fine even if already signaled

main()
  └─ pthread_join(collector,NULL) and exit
```



