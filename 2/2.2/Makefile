# Компилятор и параметры компиляции.
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g

# Имя исполняемого файла и объектные файлы.
TARGET = calculator
OBJECTS = main.o calc.o

# Цель по умолчанию.
all: $(TARGET)

# Компоновка: из двух .o получаем одну программу.
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

# Раздельная компиляция интерфейса и логики.
main.o: main.c calc.h
	$(CC) $(CFLAGS) -c main.c -o main.o

calc.o: calc.c calc.h
	$(CC) $(CFLAGS) -c calc.c -o calc.o

# Собрать при необходимости и запустить.
run: $(TARGET)
	./$(TARGET)

# Удалить файлы, созданные при сборке.
clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all run clean
