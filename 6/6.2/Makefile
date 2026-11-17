CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g
PICFLAGS = -fPIC
all: phonebook
libcontacts.so: contacts.o
	$(CC) -shared -o $@ $^
contacts.o: contacts.c contacts.h
	$(CC) $(CFLAGS) $(PICFLAGS) -c $< -o $@
main.o: main.c contacts.h
	$(CC) $(CFLAGS) -c $< -o $@
phonebook: main.o libcontacts.so
	$(CC) main.o -L. -lcontacts -Wl,-rpath,'$$ORIGIN' -o $@
run: phonebook
	./phonebook
clean:
	rm -f main.o contacts.o libcontacts.so phonebook
.PHONY: all run clean
