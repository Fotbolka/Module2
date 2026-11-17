CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g
all: phonebook
phonebook: main.o contacts.o
	$(CC) $^ -o $@
main.o: main.c contacts.h
	$(CC) $(CFLAGS) -c $< -o $@
contacts.o: contacts.c contacts.h
	$(CC) $(CFLAGS) -c $< -o $@
run: phonebook
	./phonebook
clean:
	rm -f main.o contacts.o phonebook
.PHONY: all run clean
