CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g
AR = ar
all: phonebook
libcontacts.a: contacts.o
	$(AR) rcs $@ $^
contacts.o: contacts.c contacts.h
	$(CC) $(CFLAGS) -c $< -o $@
main.o: main.c contacts.h
	$(CC) $(CFLAGS) -c $< -o $@
phonebook: main.o libcontacts.a
	$(CC) main.o ./libcontacts.a -o $@
run: phonebook
	./phonebook
clean:
	rm -f main.o contacts.o libcontacts.a phonebook
.PHONY: all run clean
