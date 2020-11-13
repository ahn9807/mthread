/**
 * Implementing 
 * 1 vs all user thread
 * Implemented by junho ahn by NOV 13 2020
 */
#include "mthread.h"
#include "queue.h"
#include "mscheduler.h"
#include <stdlib.h>
#include <ucontext.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

void mthread_wrapper(void(*func)(void*), void* arg) {
    func(arg);
}

int mthread_init() {
    //Initialize queue
    ready_queue = malloc(sizeof(queue_t));
    terminated_queue = malloc(sizeof(queue_t));
    sleep_queue = malloc(sizeof(queue_t));
}

int mthread_free() {
    free(ready_queue);
    free(terminated_queue);
    free(sleep_queue);

    return 0;
}

int mthread_create(mthread_t* thread, void(*func)(void*), void* arg) {
    //Memory Allocation
    mthread_tcb_t* tcb = malloc(sizeof(mthread_tcb_t));
    ucontext_t* cnt = malloc(sizeof(ucontext_t));
    void* sp = malloc(MAX_STACK_SIZE);

    if(tcb == NULL || cnt == NULL || sp == NULL) {
        free(tcb);
        free(cnt);
        free(sp);
        return -1;
    }
    
    getcontext(cnt);

    //Make context
    cnt->uc_stack.ss_sp = sp;
    cnt->uc_stack.ss_size = MAX_STACK_SIZE;
    cnt->uc_stack.ss_flags = 0;

    //Set Tcb
    tcb->state = READY;
    tcb->tid = tid++;
    tcb->context = cnt;
    tcb->stack = sp;
    tcb->parent_thread = mthread_current();
    if(mthread_current() != NULL) {
       if(mthread_current()->child_threads == NULL) {
           mthread_current()->child_threads = malloc(sizeof(queue_t));
       }

        insert_head(mthread_current()->child_threads, tcb);
    }

    makecontext(cnt, (void (*)(void))mthread_wrapper, 2, func, arg);

    //enqueue
    insert_tail(ready_queue, tcb);

    return 0;
}

void mthread_start() {
    running_thread = get_head(ready_queue);
    running_thread->state = RUNNING;
    setcontext(running_thread->context);
}

void mthread_exit(void* retval) {
    mthread_tcb_t* current_t = mthread_current();
    current_t->return_value = retval;
    current_t->state = TERMINATED;

    free(current_t->stack);
    free(current_t->context);

    insert_head(terminated_queue, current_t);

    mthread_yield();
}

int mthread_yield() {
    fifo_scheduler(0);
}

int mthread_join(mthread_t th, void **thread_return) {
    node_t* iter = mthread_current()->child_threads->head;

    while(iter != NULL) {
        if(((mthread_tcb_t*)iter->data)->tid == th) {
            return 0;
        }

        iter = iter->prev_node;
    }

    mthread_yield();
}

mthread_t mthread_self() {
    return mthread_current()->tid;
}