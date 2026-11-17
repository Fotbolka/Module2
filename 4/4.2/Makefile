CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g
all: priority_queue
priority_queue: main.o queue.o
	$(CC) $^ -o $@
main.o: main.c queue.h
	$(CC) $(CFLAGS) -c $< -o $@
queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c $< -o $@
run: priority_queue
	./priority_queue
clean:
	rm -f main.o queue.o priority_queue
.PHONY: all run clean
