#include "calc.h"
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

void registry_init(Registry *r) {
    if (r) {
        r->items = NULL;
        r->count = 0;
        r->capacity = 0;
    }
}

int registry_add(Registry *r, const char *name, char symbol, CalcFunction fn) {
    if (!r || !name || !fn) return 0;
    for (size_t i = 0; i < r->count; ++i)
        if (r->items[i].symbol == symbol) return 0;

    if (r->count == r->capacity) {
        size_t cap = r->capacity ? r->capacity * 2 : 2;
        if (cap < r->capacity || cap > SIZE_MAX / sizeof(*r->items)) return 0;
        Operation *new_items = realloc(r->items, cap * sizeof(*new_items));
        if (!new_items) return 0;
        r->items = new_items;
        r->capacity = cap;
    }
    r->items[r->count++] = (Operation){name, symbol, fn};
    return 1;
}

int registry_remove(Registry *r, char symbol) {
    if (!r) return 0;
    for (size_t i = 0; i < r->count; ++i) {
        if (r->items[i].symbol == symbol) {
            for (size_t j = i + 1; j < r->count; ++j)
                r->items[j - 1] = r->items[j];
            --r->count;
            return 1;
        }
    }
    return 0;
}

void registry_free(Registry *r) {
    if (r) {
        free(r->items);
        registry_init(r);
    }
}

static int store(double value, double *out) {
    if (!out || !isfinite(value)) return 0;
    *out = value;
    return 1;
}

int calc_add(double a, double b, double *out) { return store(a + b, out); }
int calc_subtract(double a, double b, double *out) { return store(a - b, out); }
int calc_multiply(double a, double b, double *out) { return store(a * b, out); }
int calc_divide(double a, double b, double *out) { return b != 0.0 && store(a / b, out); }
