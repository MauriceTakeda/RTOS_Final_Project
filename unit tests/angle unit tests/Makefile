UNAME=$(shell uname)

CCFLAGS=-Wall -O3 -std=gnu99
CC=gcc

all: angle_task

angle_task: main.o angle_task.o ctest.h angle_task_tests.o
	$(CC) $(LDFLAGS) main.o angle_task.o angle_task_tests.o -o angletask -lm

remake: clean all

%.o: %.c ctest.h
	$(CC) $(CCFLAGS) -c -o $@ $<

clean:
	rm -f angletask *.o
