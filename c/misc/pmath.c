#include "pmath.h"

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

int gcd (int a, int b) {
    if (a==0) return b;
    return gcd (b%a, a);
}

int ppcm (int X, int Y) {
    int A = X;
    int B = Y;

    while (A!=B) {
        while (A>B) B=B+Y;
        while (A<B) A=A+X;
    }
    return A;
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif