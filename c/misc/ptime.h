#ifndef USEFUL_LIB_MISC_STRING
#define USEFUL_LIB_MISC_STRING

#include <stdlib.h>
#include <time.h>

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

struct timespec timespec_diff(struct timespec start, struct timespec end);
long long int timespec_get_nanosecond(struct timespec timer);

#if _WIN32
    #include <windows.h>
    #include "minwinbase.h"

    // CLOCK_PROCESS_CPUTIME_ID is not defined on Windows, we just redefine it
    #define CLOCK_PROCESS_CPUTIME_ID 0
    
    // From https://stackoverflow.com/questions/6649936
    //struct timespec { long tv_sec; long tv_nsec; };   //header part

    void unix_time(struct timespec *spec);
    int clock_gettime(int a, struct timespec *spec);
#endif

void _get_precision_timer(struct timespec *spec);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif

