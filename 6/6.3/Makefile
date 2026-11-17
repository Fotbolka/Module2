CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g
PICFLAGS = -fPIC
PLUGINS = plugins/add.so plugins/subtract.so plugins/multiply.so plugins/divide.so
all: calculator $(PLUGINS)
calculator: main.o loader.o
	$(CC) $^ -ldl -o $@
main.o: main.c loader.h plugin_api.h
	$(CC) $(CFLAGS) -c $< -o $@
loader.o: loader.c loader.h plugin_api.h
	$(CC) $(CFLAGS) -c $< -o $@
plugins/%.so: plugins/%.c plugin_api.h
	$(CC) $(CFLAGS) $(PICFLAGS) -shared $< -o $@
run: all
	./calculator ./plugins
clean:
	rm -f calculator main.o loader.o $(PLUGINS)
.PHONY: all run clean
