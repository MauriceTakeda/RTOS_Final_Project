UNAME=$(shell uname)

CCFLAGS=-Wall -O3 -std=gnu99
CC=gcc

all: led_task

led_task: main.o led_task.o ctest.h led_task_tests.o
	$(CC) $(LDFLAGS) main.o led_task.o led_task_tests.o -o ledtask -lm

remake: clean all

%.o: %.c ctest.h
	$(CC) $(CCFLAGS) -c -o $@ $<

clean:
	rm -f ledtask *.o
