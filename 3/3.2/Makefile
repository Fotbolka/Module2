CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g
all: network
network: main.o network.o
	$(CC) $^ -o $@
main.o: main.c network.h
	$(CC) $(CFLAGS) -c $< -o $@
network.o: network.c network.h
	$(CC) $(CFLAGS) -c $< -o $@
run: network
	./network 192.168.1.1 255.255.255.0 10
clean:
	rm -f main.o network.o network
.PHONY: all run clean
