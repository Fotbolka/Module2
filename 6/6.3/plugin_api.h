#ifndef PLUGIN_API_H
#define PLUGIN_API_H
/* Shared ABI: one mathematical operation in each .so plugin. */
typedef int (*CalculateFn)(double a,double b,double *out);
typedef struct {
    const char *name;
    const char *symbol;
    CalculateFn calculate;
} CalcPlugin;
/* Every .so exports: const CalcPlugin *calculator_plugin(void); */
typedef const CalcPlugin *(*PluginEntry)(void);
#endif
