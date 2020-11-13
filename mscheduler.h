#ifndef _MSCHEDULER_H_
#define _MSCHEDULER_H_

#include "mthread.h"
#include "queue.h"

void fifo_scheduler(int signum);
mthread_tcb_t* mthread_current(void);
mthread_tcb_t* mthread_next(void);

#endif