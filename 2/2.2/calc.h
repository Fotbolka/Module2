#ifndef CALC_H
#define CALC_H

/* Математические операции: здесь только объявления функций. */
double calc_add(double a, double b);
double calc_subtract(double a, double b);
double calc_multiply(double a, double b);

/* Возвращает 1 при успехе, 0 при делении на ноль. */
int calc_divide(double a, double b, double *result);

#endif /* CALC_H */
