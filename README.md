*This project has been created as part of the 42 curriculum by slidriss.*

# Codexion

## Description

Codexion is a concurrency simulation inspired by the classic **Dining Philosophers** problem. N coders sit in a circle around a shared Quantum Compiler. Each coder needs two USB dongles (one in each hand) to compile. Dongles are shared: there is exactly one dongle between each pair of adjacent coders.

Each coder loops through three phases:
1. **Compile** – acquire two dongles, compile for `time_to_compile` ms, release dongles
2. **Debug** – spend `time_to_debug` ms debugging
3. **Refactor** – spend `time_to_refactor` ms refactoring, then restart

If a coder does not start compiling within `time_to_burnout` ms since their last compile (or since the simulation start), they **burn out** and the simulation ends.

The simulation also ends normally when every coder has compiled at least `number_of_compiles_required` times.

## Instructions

### Compilation

```bash
make
```

### Execution

```bash
./codexion nb_coders time_to_burnout time_to_compile time_to_debug time_to_refactor nb_compiles_required dongle_cooldown scheduler
```

All arguments are mandatory. `scheduler` must be exactly `fifo` or `edf`.

### Examples

```bash
# 4 coders, burnout at 400ms, compile 200ms, debug 200ms, refactor 400ms, 5 compiles each, 0ms cooldown, FIFO
./codexion 4 400 200 200 400 5 0 fifo

# Same but EDF scheduler
./codexion 4 400 200 200 400 5 0 edf

# 1 coder (no deadlock possible)
./codexion 1 800 200 200 200 3 50 fifo
```

### Expected output format

```
0 1 has taken a dongle
2 1 has taken a dongle
2 1 is compiling
202 1 is debugging
...
1505 4 burned out
```

## Resources

- [POSIX Threads Programming – Lawrence Livermore National Laboratory](https://hpc-tutorials.llnl.gov/posix/)
- [Dining Philosophers Problem – Wikipedia](https://en.wikipedia.org/wiki/Dining_philosophers_problem)
- [pthread_cond_timedwait – man page](https://man7.org/linux/man-pages/man3/pthread_cond_timedwait.3p.html)
- [Earliest Deadline First scheduling – Wikipedia](https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling)
- [Binary heap – Wikipedia](https://en.wikipedia.org/wiki/Binary_heap)

### AI usage

Claude (Anthropic) was used to:
- Explain POSIX threading concepts (mutex, condition variables, timedwait)
- Review the deadlock prevention strategy (asymmetric dongle acquisition order)
- Clarify the EDF scheduling algorithm and tie-breaker rules
- Suggest the per-request condition variable pattern for fair wakeup

All generated code was reviewed, understood, and validated line by line before inclusion.

## Blocking cases handled

### Deadlock prevention (Coffman's conditions)
A deadlock requires four conditions: mutual exclusion, hold-and-wait, no preemption, and circular wait. We break the **circular wait** condition using the asymmetric acquisition strategy: all coders except the last one take their left dongle first then their right dongle. The last coder takes them in reverse order (right then left). This breaks the circular dependency chain that would otherwise form when every coder holds their left dongle and waits for their right one.

### Starvation prevention
Under FIFO scheduling, each coder is served strictly in arrival order, guaranteeing no starvation as long as parameters are feasible. Under EDF, coders with the earliest burnout deadline are served first. A liveness guarantee holds as long as `time_to_compile + 2 * dongle_cooldown < time_to_burnout`, because a coder will always eventually become the most urgent and be served.

### Cooldown handling
After a dongle is released, it is marked unavailable until `now + dongle_cooldown` milliseconds. The next waiter uses `pthread_cond_timedwait` with an absolute timeout equal to `available_at`, so it sleeps precisely until the cooldown expires without busy-waiting.

### Precise burnout detection
A dedicated monitor thread polls all coders' `last_compile_start` timestamps every 1 ms. When `now - last_compile_start[i] >= time_to_burnout`, the monitor immediately prints the burnout message and sets `running = 0`. The 1 ms polling interval ensures the message is displayed well within the required 10 ms tolerance.

### Log serialization
All `printf` calls are wrapped in `ft_log()` and `ft_log_burnout()`, which lock a shared `log_mutex` before writing. This guarantees that no two log lines ever interleave, even when many threads print simultaneously.

## Thread synchronization mechanisms

### `pthread_mutex_t`
- **`log_mutex`**: serializes all output. Locked around every `printf` call.
- **`running_mutex`**: protects the `running` flag shared between all threads and the monitor.
- **`dongle[i].mutex`**: protects each dongle's state (`in_use`, `available_at`, `queue`). Coders lock this mutex before pushing their request into the heap or checking dongle availability.

### `pthread_cond_t` (per-request condition variable)
Each `t_request` has its own `pthread_cond_t cond`. When a coder pushes its request into a dongle's heap, it calls `pthread_cond_wait(&req.cond, &dongle.mutex)` and sleeps. When the current dongle holder calls `dongle_release()`, it pops the next request from the heap and calls `pthread_cond_signal(&next_req->cond)` — waking only the correct next coder. This prevents the thundering herd problem where all waiters would wake up simultaneously.

### `pthread_cond_timedwait`
Used when a coder is next in the queue but the dongle cooldown has not yet expired. Instead of busy-waiting, the coder computes the exact absolute `struct timespec` for `available_at` and sleeps until that moment. It is woken earlier if another signal arrives (e.g., if the simulation stops).

### Race condition prevention example
Without `log_mutex`:
- Thread 1 calls `printf("100 1 is comp`)
- Thread 2 calls `printf("100 2 is debugging\n")`
- Output: `100 1 is comp100 2 is debugging\niling\n` ← corrupted

With `log_mutex`, each `printf` is atomic from the perspective of other threads.

### Monitor–coder communication
The monitor sets `sim->running = 0` under `running_mutex`. Each coder checks `is_running()` (which locks `running_mutex`) between phases. This is the only communication channel: coders do not need to know why the simulation stopped, only that it did.