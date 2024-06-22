#include "lwp.h"
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

static schedfun scheduler = NULL;

lwp_context lwp_ptable[LWP_PROC_LIMIT];
int lwp_procs = 0;
int lwp_running = 0;
int start = 1;
void *sp_restorestate;

int default_scheduler() {
    // Default round-robin scheduling logic
    if(start == 1){
        start = 0;
        return lwp_running; // 0
    }

    lwp_running = (lwp_running + 1) % lwp_procs; // lwp_procs() returns total number of LWPs
    return lwp_running;
}  

int new_lwp(lwpfun function, void *argument, size_t stacksize) {

    if (lwp_procs == LWP_PROC_LIMIT){
        return -1;
    } 

    // if scheduler has never been set, the library should do round robin scheduling
    if(scheduler == NULL){
        lwp_set_scheduler(default_scheduler);
    }

    ptr_int_t *sp, *stack, bp;
    int pid;

    // Allocating Space in Stack
    stack = (ptr_int_t *) malloc(sizeof(ptr_int_t *) * stacksize);

    if (stack == NULL) {
        // Memory allocation failed
        return -1;
    }

    sp = stack;
    sp += stacksize;
    sp --;

    *sp = (ptr_int_t) argument; // argument
    sp --;
    *sp = (ptr_int_t) 0xFEEDBEEF; // bogus return value
    sp --;

    // Fake Stack for Main()
    *sp = (ptr_int_t) function; // this is set to return address
    sp --;
    *sp = (ptr_int_t) 0xFEEDBEEF; // bogus bp

    // Creating Space for Thread to Store Information before Yield
    bp = (ptr_int_t) sp;
    sp -= 7;
    *sp = (ptr_int_t) bp;

    // Set ESP
    // SetSP(sp);

    // Create space for struct (process table entry)
    lwp_context *new_lwp_context = (lwp_context *) malloc(sizeof(lwp_context));
    if (new_lwp_context == NULL) {
        // Memory allocation failed
        return -1;
    }

    new_lwp_context->pid = lwp_procs;
    pid = lwp_procs;
    new_lwp_context->stack = stack;
    new_lwp_context->stacksize = stacksize;
    new_lwp_context->sp = sp;

    lwp_ptable[lwp_procs++] = *new_lwp_context;
    free(new_lwp_context);

    return pid;
}

void lwp_set_scheduler(schedfun sched) {
    // Set the new scheduling function
    if(sched != NULL){
        scheduler = sched;
    }
    else{
        scheduler = default_scheduler;
    }
}
void lwp_exit(){
    // Terminates the current LWP
    free(lwp_ptable[lwp_running].stack);
    // shift everything in ptable to take its place
    // and update # of threads
    int i = 0;
    for(i = lwp_running + 1; i < lwp_procs; i++){
        lwp_ptable[i-1] = lwp_ptable[i];
    }
    lwp_procs = lwp_procs - 1;
    start = 1;
    // terminating condition
    if(lwp_procs == 0){
        lwp_stop();
    }
    else {
        lwp_running = scheduler();
        SetSP(lwp_ptable[lwp_running].sp);
        RESTORE_STATE();
    }
}

int lwp_getpid(){
    return lwp_ptable[lwp_running].pid;
}

void lwp_yield(){
    // Save the state of the current thread 
    // (including sp to come back to correct line when restored)
    SAVE_STATE();
    GetSP(lwp_ptable[lwp_running].sp);
    // Call your scheduler function to choose the next thread
    int next_lwp = scheduler();
    // If the next thread is different from the current one, switch context
    if (next_lwp != lwp_running) {
        // Set lwp_running to the next thread
        lwp_running = next_lwp;
    }
    // Restore the state of the next thread
    SetSP(lwp_ptable[lwp_running].sp);
    RESTORE_STATE();
}

void lwp_start(){
    // Save the original context and stack pointer
    SAVE_STATE();
    GetSP(sp_restorestate);
    // Schedule an LWP
    int next_lwp = scheduler();
    // If there are no LWPs, return immediately
    if (lwp_procs == 0) {
        return;
    }
    // Start the LWP running
    lwp_running = next_lwp;
    SetSP(lwp_ptable[lwp_running].sp);
    RESTORE_STATE();
}

void lwp_stop(){
   SAVE_STATE();
   SetSP(sp_restorestate);
   RESTORE_STATE(); 
}