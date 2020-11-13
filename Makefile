CC = gcc
CFLAGS = -g

all: Test

Test: test1.c 
	$(CC) $(CFLAGS) mthread.c queue.c mscheduler.c test1.c -o test1


clean:
	rm -f Test