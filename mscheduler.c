/**
 * Implementing FIFO Scheduler for user thread
 */
#include "mthread.h"
#include "mscheduler.h"
#include "queue.h"

void fifo_scheduler(int signum) {
    //when ready queue is empty
    if(is_empty(ready_queue)) {
        return;
    }

    mthread_tcb_t* current_thread = mthread_current();
    mthread_tcb_t* next_thread = mthread_next();
    ucontext_t* cuurent_thread_context = current_thread->context;
    ucontext_t* next_thread_context =  next_thread->context;

    if(current_thread->state != RUNNING) {
        //current thread is extied... or not existed
    } else {
        current_thread->state = READY;
        insert_tail(ready_queue, current_thread);
    }

    next_thread->state = RUNNING;
    running_thread = next_thread;

    remove_head(ready_queue); //pop next_thread

    //debug_queue(ready_queue);

    swapcontext(cuurent_thread_context, next_thread_context);
}

mthread_tcb_t* mthread_current(void) {
    return running_thread;
}

mthread_tcb_t* mthread_next(void) {
    if(peek_head(ready_queue) == NULL)
        return running_thread;
    else 
        return (mthread_tcb_t*)(peek_head(ready_queue));
}