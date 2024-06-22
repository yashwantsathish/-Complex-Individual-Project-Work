# Project Description:

Implementation of a Lightwight Process (LWP) Library in C, which allows for the creation and management of lightweight processes (also known as user-level threads).

These LWPs are scheduled using a customizable scheduling function, with a default round-robin scheduler provided.

## Key Components

### Data Structures and Variables

- **lwp_context**: Struct to store the context of an LWP, including the stack pointer, stack size, and process ID.
- **lwp_ptable**: Array to store all the LWPs' contexts.
- **lwp_procs**: Count of currently active LWPs.
- **lwp_running**: Index of the currently running LWP.
- **scheduler**: Function pointer to the current scheduling function.
- **sp_restorestate**: Pointer to the stack state to restore when stopping LWPs.

### Default Scheduler

- **default_scheduler()**: Implements a simple round-robin scheduling logic.

### Creating a New LWP

- **new_lwp(lwpfun function, void *argument, size_t stacksize)**: Allocates a new LWP, sets up its stack, and adds it to the process table.

### Setting a Scheduler

- **lwp_set_scheduler(schedfun sched)**: Allows the user to set a custom scheduler. If none is provided, it defaults to `default_scheduler`.

### Exiting an LWP

- **lwp_exit()**: Terminates the currently running LWP, frees its resources, and schedules the next LWP.

### Getting the Current LWP PID

- **lwp_getpid()**: Returns the process ID of the currently running LWP.

### Yielding Execution

- **lwp_yield()**: Saves the current LWP's state, calls the scheduler to select the next LWP, and switches context to the next LWP.

### Starting LWP Execution

- **lwp_start()**: Saves the current context, selects an LWP to run using the scheduler, and starts executing it.

### Stopping LWP Execution

- **lwp_stop()**: Stops the currently running LWP and restores the original context saved by `lwp_start()`.
lwp_stop(): Restores the original state saved in lwp_start(), effectively stopping LWP execution and returning to the original context.
