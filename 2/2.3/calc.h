#ifndef CALC_H
#define CALC_H

#include <stddef.h>

typedef int (*CalcFunction)(double, double, double *);

typedef struct {
    const char *name;
    char symbol;
    CalcFunction function;
} Operation;

typedef struct {
    Operation *items;
    size_t count;
    size_t capacity;
} Registry;

void registry_init(Registry *r);
int registry_add(Registry *r, const char *name, char symbol, CalcFunction fn);
int registry_remove(Registry *r, char symbol);
void registry_free(Registry *r);

int calc_add(double a, double b, double *out);
int calc_subtract(double a, double b, double *out);
int calc_multiply(double a, double b, double *out);
int calc_divide(double a, double b, double *out);

#endif
