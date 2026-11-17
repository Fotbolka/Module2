CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g
all: permissions
permissions: main.o permissions.o
	$(CC) $^ -o $@
main.o: main.c permissions.h
	$(CC) $(CFLAGS) -c $< -o $@
permissions.o: permissions.c permissions.h
	$(CC) $(CFLAGS) -c $< -o $@
run: permissions
	./permissions
clean:
	rm -f main.o permissions.o permissions
.PHONY: all run clean
