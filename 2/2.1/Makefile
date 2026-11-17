# Компилятор и параметры компиляции.
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g

# Результат сборки и объектные файлы.
TARGET = phonebook
OBJECTS = main.o contacts.o

# Первая цель выполняется при вызове команды make без аргументов.
all: $(TARGET)

# Компоновка (связывание) двух объектных файлов в программу.
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

# Раздельная компиляция: исходники и заголовочный файл — зависимости.
main.o: main.c contacts.h
	$(CC) $(CFLAGS) -c main.c -o main.o

contacts.o: contacts.c contacts.h
	$(CC) $(CFLAGS) -c contacts.c -o contacts.o

# Удобные вспомогательные цели.
run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all run clean
