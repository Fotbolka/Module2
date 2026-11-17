#include "calc.h"

#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Считывает строку: 1 — успех, 0 — конец ввода, -1 — слишком длинная. */
static int read_line(const char *prompt, char *buffer, size_t capacity)
{
    printf("%s", prompt);
    fflush(stdout);

    if (fgets(buffer, (int)capacity, stdin) == NULL) {
        return 0;
    }

    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
        return 1;
    }
    if (feof(stdin)) {
        return 1;
    }

    /* Строка не поместилась: удаляем остаток, чтобы не испортить новый ввод. */
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
    puts("Слишком длинный ввод. Попробуйте снова.");
    return -1;
}

/* Проверяет пункт меню, а не просто доверяет введённому тексту. */
static int read_menu_choice(int *choice)
{
    char buffer[128];

    for (;;) {
        int status = read_line("Выберите действие: ", buffer, sizeof(buffer));
        if (status == 0) {
            return 0;
        }
        if (status == -1) {
            continue;
        }

        char *end;
        errno = 0;
        long value = strtol(buffer, &end, 10);
        while (isspace((unsigned char)*end)) {
            ++end;
        }
        if (errno != ERANGE && end != buffer && *end == '\0'
                && value >= 0 && value <= 4) {
            *choice = (int)value;
            return 1;
        }
        puts("Ошибка: введите целое число от 0 до 4.");
    }
}

/* Читает вещественное число; десятичный разделитель — точка. */
static int read_number(const char *prompt, double *number)
{
    char buffer[128];

    for (;;) {
        int status = read_line(prompt, buffer, sizeof(buffer));
        if (status == 0) {
            return 0;
        }
        if (status == -1) {
            continue;
        }

        char *end;
        errno = 0;
        double value = strtod(buffer, &end);
        while (isspace((unsigned char)*end)) {
            ++end;
        }
        if (errno == 0 && end != buffer && *end == '\0' && isfinite(value)) {
            *number = value;
            return 1;
        }
        puts("Ошибка: введите число, например 12 или 3.5 (через точку).");
    }
}

int main(void)
{
    for (;;) {
        puts("\n===== КАЛЬКУЛЯТОР =====");
        puts("1 — Сложение (+)");
        puts("2 — Вычитание (-)");
        puts("3 — Умножение (*)");
        puts("4 — Деление (/)");
        puts("0 — Выход");

        int choice;
        if (!read_menu_choice(&choice) || choice == 0) {
            puts("Завершение программы.");
            break;
        }

        double a, b, result;
        if (!read_number("Первое число: ", &a)
                || !read_number("Второе число: ", &b)) {
            puts("Ввод завершён.");
            break;
        }

        /* В задании 2.2 операции выбираются обычным switch.
           В задании 2.3 заменим этот выбор указателями на функции. */
        switch (choice) {
            case 1:
                result = calc_add(a, b);
                break;
            case 2:
                result = calc_subtract(a, b);
                break;
            case 3:
                result = calc_multiply(a, b);
                break;
            case 4:
                if (!calc_divide(a, b, &result)) {
                    puts("Ошибка: деление на ноль запрещено.");
                    continue;
                }
                break;
            default:
                continue;
        }

        if (!isfinite(result)) {
            puts("Ошибка: результат выходит за диапазон вещественного числа.");
            continue;
        }
        printf("Результат: %.10g\n", result);
    }

    return 0;
}
