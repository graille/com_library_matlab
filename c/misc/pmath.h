#ifndef USEFUL_LIB_MATH_H
#define USEFUL_LIB_MATH_H
 
#include <stdlib.h>
#include <math.h>

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#ifndef max
    #define max(a,b) \
    ({ __typeof__ (a) _a = (a); \
        __typeof__ (b) _b = (b); \
        _a > _b ? _a : _b; })
#endif

#ifndef min
    #define min(a,b) \
    ({ __typeof__ (a) _a = (a); \
        __typeof__ (b) _b = (b); \
        _a < _b ? _a : _b; })
#endif

int gcd (int a, int b);
int ppcm (int a, int b);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif // USEFUL_LIB_MATH_H