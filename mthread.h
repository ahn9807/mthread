#ifndef MTHREAD_H
#define MTHREAD_H

#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#define MAX_STACK_SIZE (8 * 1024)

u_int64_t tid;
typedef u_int8_t mthread_t;

typedef enum {
    RUNNING, READY, WAITHING, SLEEP, TERMINATED,
} mthread_state_t;

typedef struct _mthread_tcb {
    ucontext_t* context;
    mthread_state_t state;
    mthread_t tid;
    void* stack;
    void* return_value;
    struct _mthread_tcb* next_thread;
    struct _mthread_tcb* prev_thread;
} mthread_tcb_t;

queue_t* running_queue; //unused
queue_t* ready_queue;
queue_t* terminated_queue;
queue_t* sleep_queue; //unused
mthread_tcb_t* running_thread;

int mthread_init();
int mthread_free();
int mthread_create(mthread_t* thread, void(*function)(void*), void* arg);
void mthread_start();
void mthread_exit(void* retval);
int mthread_yield();
int mthread_join(mthread_t th, void **thread_return);
mthread_t mthread_self();
void mthread_wrapper(void(*function)(void*), void* arg);

#endif