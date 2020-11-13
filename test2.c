#include <stdio.h>
#include "mthread.h"

void thread2(void *argv) {
    printf("thread2 1\n");
    mthread_yield();
    printf("thread2 2\n");
    mthread_exit(NULL);
}

void thread3(void *argv) {
    printf("thread3 1\n");
    mthread_yield();
    printf("thread3 2\n");
    mthread_exit(NULL);
}

void thread1(void *argv) {
    printf("thread1 1\n");
    mthread_t thread3_tid;
    mthread_t thread2_tid;
    mthread_create(&thread2_tid, thread2, NULL);
    mthread_create(&thread3_tid, thread3, NULL);
    
    mthread_join(thread2_tid, NULL);
    mthread_join(thread3_tid, NULL);
    mthread_yield();
    printf("thread1 2\n");
    mthread_exit(NULL);
}

int main() {
    mthread_init();
    mthread_t thread1_tid;
    mthread_t thread2_tid;
    mthread_t thread3_tid;
    
    mthread_create(&thread1_tid, thread1, NULL);
    mthread_create(&thread2_tid, thread2, NULL);
    mthread_create(&thread3_tid, thread3, NULL);

    mthread_start();
    mthread_join(thread1_tid, NULL);
    mthread_join(thread2_tid, NULL);
    mthread_join(thread3_tid, NULL);

    mthread_free();

    return 0;
}