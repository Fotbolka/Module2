CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g
LDLIBS = -lm

all: calculator

calculator: main.o calc.o
	$(CC) main.o calc.o -o $@ $(LDLIBS)

main.o: main.c calc.h
	$(CC) $(CFLAGS) -c main.c -o $@

calc.o: calc.c calc.h
	$(CC) $(CFLAGS) -c calc.c -o $@

clean:
	rm -f main.o calc.o calculator

.PHONY: all clean
