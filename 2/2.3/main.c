#include "calc.h"
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static const Operation available_operations[] = {
    {"Сложение", '+', calc_add},
    {"Вычитание", '-', calc_subtract},
    {"Умножение", '*', calc_multiply},
    {"Деление", '/', calc_divide},
    {"Уложение", '&', calc_add},
};

static const size_t available_count =
    sizeof(available_operations) / sizeof(available_operations[0]);

static int line(const char *prompt, char *buf, size_t size) {
    printf("%s", prompt);
    fflush(stdout);
    if (!fgets(buf, (int)size, stdin)) return 0;
    size_t n = strlen(buf);
    if (n && buf[n - 1] == '\n') {
        buf[n - 1] = '\0';
        return 1;
    }
    if (feof(stdin)) return 1;
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
    puts("Слишком длинный ввод.");
    return -1;
}

static int integer(const char *prompt, int min, int max, int *out) {
    char buf[128];
    for (;;) {
        int st = line(prompt, buf, sizeof(buf));
        if (st == 0) return 0;
        if (st < 0) continue;
        char *end;
        errno = 0;
        long x = strtol(buf, &end, 10);
        while (isspace((unsigned char)*end)) ++end;
        if (errno != ERANGE && end != buf && !*end && x >= min && x <= max) {
            *out = (int)x;
            return 1;
        }
        printf("Введите число от %d до %d.\n", min, max);
    }
}

static int number(const char *prompt, double *out) {
    char buf[128];
    for (;;) {
        int st = line(prompt, buf, sizeof(buf));
        if (st == 0) return 0;
        if (st < 0) continue;
        char *end;
        errno = 0;
        double x = strtod(buf, &end);
        while (isspace((unsigned char)*end)) ++end;
        if (errno == 0 && end != buf && !*end && isfinite(x)) {
            *out = x;
            return 1;
        }
        puts("Введите корректное конечное число (десятичная точка).");
    }
}

static const Operation *find_available(char symbol) {
    for (size_t i = 0; i < available_count; ++i)
        if (available_operations[i].symbol == symbol)
            return &available_operations[i];
    return NULL;
}

int main(int argc, char **argv) {
    Registry r;
    registry_init(&r);

    if (argc == 1) {
        for (size_t i = 0; i < available_count; ++i) {
            const Operation *op = &available_operations[i];
            if (!registry_add(&r, op->name, op->symbol, op->function)) goto error;
        }
    } else {
        for (int i = 1; i < argc; ++i) {
            if (strlen(argv[i]) != 1) {
                fprintf(stderr, "Ошибка: нужен один символ операции: %s\n", argv[i]);
                goto error;
            }
            const Operation *op = find_available(argv[i][0]);
            if (!op || !registry_add(&r, op->name, op->symbol, op->function)) {
                fprintf(stderr, "Ошибка: неизвестная или повторная операция: %s\n", argv[i]);
                goto error;
            }
        }
    }

    for (;;) {
        puts("\n===== КАЛЬКУЛЯТОР (указатели на функции) =====");
        for (size_t i = 0; i < r.count; ++i)
            printf("%zu — %s (%c)\n", i + 1, r.items[i].name, r.items[i].symbol);
        puts("0 — Выход");
        int choice;
        if (!integer("Выберите действие: ", 0, (int)r.count, &choice) || choice == 0)
            break;
        double a, b, out;
        if (!number("Первое число: ", &a) || !number("Второе число: ", &b))
            break;
        Operation *op = &r.items[(size_t)choice - 1];
        if (!op->function(a, b, &out))
            puts("Операция невозможна (деление на ноль или переполнение).");
        else
            printf("Результат: %.10g\n", out);
    }

    registry_free(&r);
    return 0;
error:
    registry_free(&r);
    return 1;
}
