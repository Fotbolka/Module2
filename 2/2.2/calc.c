#include "calc.h"

#include <stddef.h>

/* Логическая часть: никакого ввода и вывода. */
double calc_add(double a, double b)
{
    return a + b;
}

double calc_subtract(double a, double b)
{
    return a - b;
}

double calc_multiply(double a, double b)
{
    return a * b;
}

int calc_divide(double a, double b, double *result)
{
    if (result == NULL || b == 0.0) {
        return 0;
    }

    *result = a / b;
    return 1;
}
