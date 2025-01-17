## user level thread
### queue.c 
스레드 스케쥴링에 필요한 queue 자료 구조를 정의한다. 메모리 낭비를 막기 위해서 linux처럼 linked list를 통해서 구현하였다. 각각의 노드들의 전 노드와 다음 노드에 대한 포인터 주소를 저장하며, 데이터가 현재 mthread_tcb_t의 주소값을 저장하도록 하여 tcb의 큐를 구현하였다. 

### mthread.c
스레드 스케쥴링에서 필수적으로 필요한 함수인 create, yield, exit과 같은 함수를 선언하였다. 
#### int mthread_create(mthread_t *thred, void (*strt_function)(void *), void *arg)
1. 첫번째 argument인 thread 는 스레드가 성공적으로 생성되었을때 생성된 스레드를 식별하기 위해서 사용되는 스레드 식별자이다.
2. 2번째 argument인 start_routine는 분기시켜서 실행할 스레드 함수이며,
3. 3번째 argument인 arg는 스레드 함수의 인자이다.
4. 성공적으로 생성될경우 0을 리턴한다.
5. thread_tcb 구조체 생성과 메모리 할당
6. 스레드가 사용할 스택 영역 할당
7. 스레드 context 지정
8. 큐에 enqueue
   
#### mthread_t mthread_self(void)
1. 현재 이 함수를 호출한 함수에서 자기자신의 스레드 아이디를 얻고 싶을때, 스레드를 tid를 리턴한다.

#### void pthread_exit(void *retval);
1. 스레드를 종료시키고 자신을 생성한 함수에 retval을 리턴시킨다.
2. 현재 thread상태를 terminated로 변경후 thread_yield 호출

#### int pthread_yield()
1. 스케쥴러를 명시적으로 호출한다. 

#### int pthread_join(mthread_t th, void **thread_return);
사실 thread join은 현재 1 thread에서 스케쥴링이 일어나는 현 상황에서는 필요가 없다. 여기서는 리턴값의 회수라는 측면에 초점을 맞추어서 한번 실행해 보도록 하자. 또한 메인 스레드를 스레딩의 영역에 포함시킬 것인지는 구현후 생각해보도록 하자. 
1. 목표 스레드가 완수될때까지 현재 스레드를 호출한 스레드의 실행을 정지시킨다.
2. 0을 리턴하게 되며, thread_exit에서 인자로 넘겨진 retval이 명시된 thread_return을 통해서 리턴되어진다.

#### 자료구조
1. typedef enum { RUNNING, READY, WAITING, SLEEP, TERMINATED } mthread_state_t;
2. struct mthread_tcb { ucontext_t *context; mthread_state_t state; mthread_t tid; void* return_value; }
3. typedef uint8_t mthread_t; 스레드의 id 저장
4. 또한 각종 ready_queue와 같은 스케줄링을 위해 필요한 큐들

### mschedule.c
여기서는 스케쥴링 함수를 정의 한다. 제일 단순한 FIFO함수를 만들도록 하자. 또한 yield 함수가 여러번 호출될 일은 없어서 (1 thread에서 작동함으로) preemption 함수는 작성할 필요가 없다. 만약 이 user thread가 여러 논리 코어를 사용할 수 있게 된다면 구현하도록 한다. 또한 time schedular에 의해서 작동할 경우에도 preemption을 구현하여야 하지만, 또한 구현의 범위를 넘어섬으로 생략하도록 한다. 
1. 현재 thread를 READY로 변경 현재 thread가 TERMINATED면 메모리 해제후 큐에서 제거. 
2. 현재 스레드가 READY면 큐 맨 뒤로 넣기
3. 큐에서 다음 스레드의 상태를 RUNNING으로 변경
4. 다음 스레드를 현재 스레드로 지정
5. context 스위칭 실행
#### void fifo_schduler(int signum)
이 함수는 timer에서 발생한 시그널을 받아서 스케쥴링를 하거나 (구현할 필요 없음) 혹은 yield가 실행되어서 다음 스레드를 실행시키게 된다. 주의 할 점은 스택 bp, sp를 복구 해야한다는 점, 그리고 context를 설정해주어야 하는점등이 있을 것이다. 그러나 이와 같은 일들은, setcontext가 알아서 해 줌으로 신경쓸 필요는 없다. 

#### mthread_tcb* mthread_current(void)
1. 큐에서 제일 위에 있는 스레드의 구조체를 이용하여 스레드 구조체의 포인터를 넘겨준다. 

#### mthread_tcb* mthread_next(void)
1. 다음에 작동시킬 스레드의 구조체를 리턴한다. 
