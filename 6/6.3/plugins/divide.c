#include "../plugin_api.h"
#include <math.h>
#include <stddef.h>
static int calculate(double a,double b,double *out) {
    if(!out || b==0.0) return 0;
    double result=a/b;
    if(!isfinite(result))return 0;
    *out=result;return 1;
}
const CalcPlugin *calculator_plugin(void) {
    static const CalcPlugin plugin={"Деление","/",calculate};
    return &plugin;
}
