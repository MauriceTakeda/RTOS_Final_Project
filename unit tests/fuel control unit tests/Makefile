UNAME=$(shell uname)

CCFLAGS=-Wall -O3 -std=gnu99
CC=gcc

all: fuel_control_task

fuel_control_task: main.o fuel_control_task.o ctest.h fuel_control_task_tests.o
	$(CC) $(LDFLAGS) main.o fuel_control_task.o fuel_control_task_tests.o -o fuelcontroltask -lm

remake: clean all

%.o: %.c ctest.h
	$(CC) $(CCFLAGS) -c -o $@ $<

clean:
	rm -f fuelcontroltask *.o
