#include "ptime.h"

#define WINDOWS_QPC_TIMER 1

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

struct timespec timespec_diff(struct timespec start, struct timespec end)
{
    struct timespec temp;
    if ((end.tv_nsec - start.tv_nsec) < 0) {
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = (long) 1e9 + end.tv_nsec - start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp;
}

long long int timespec_get_nanosecond(struct timespec timer) {
    return (long) 1e9 * timer.tv_sec + timer.tv_nsec;
}

#if _WIN32
    #if WINDOWS_QPC_TIMER == 1
        /**
        * With QPC, we have a high precision element but it will onyl work for a difference
        **/

        // From https://stackoverflow.com/questions/5404277
        #define exp7           10000000i64     //1E+7     //C-file part
        #define exp9         1000000000i64     //1E+9
        #define w2ux 116444736000000000i64     //1.jan1601 to 1.jan1970

        void unix_time(struct timespec *spec) {  
            __int64 wintime; 
            
            GetSystemTimeAsFileTime((FILETIME*)&wintime); 
            wintime -= w2ux; 
            spec->tv_sec  = wintime / exp7;                 
            spec->tv_nsec = wintime % exp7 * (long) 100;
        }

        int clock_gettime(int a, struct timespec *spec) {  
            static struct timespec startspec; static double ticks2nano;
            static __int64 startticks, tps =0;    __int64 tmp, curticks;

            QueryPerformanceFrequency((LARGE_INTEGER*) &tmp); //some strange system can
            if (tps != tmp) { 
                tps = tmp; //init ~~ONCE         //possibly change freq ?
                QueryPerformanceCounter((LARGE_INTEGER*) &startticks);
                unix_time(&startspec); 
                ticks2nano = (double)exp9 / tps; 
            }

            QueryPerformanceCounter((LARGE_INTEGER*) &curticks); 
            curticks -=startticks;

            spec->tv_sec  = startspec.tv_sec   +         (curticks / tps);
            spec->tv_nsec = startspec.tv_nsec  + (double)(curticks % tps) * ticks2nano;

            if (!(spec->tv_nsec < exp9)) { spec->tv_sec++; spec->tv_nsec -=exp9; }
            
            return 0;
        }
    #else
        int clock_gettime(int a, struct timespec *spec)      //C-file part
        {  
            __int64 wintime; GetSystemTimeAsFileTime((FILETIME*)&wintime);
            wintime      -= 116444736000000000i64;  // 1jan1601 to 1jan1970
            spec->tv_sec  = wintime / 10000000i64;           //seconds
            spec->tv_nsec = wintime % 10000000i64 * 100;      //nano-seconds
            return 0;
        }
    #endif
#endif


void _get_precision_timer(struct timespec *spec) {
    #if _WIN32
        clock_gettime(0, spec);
    #else
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, spec);
    #endif
}


#if defined(__cplusplus) || defined(c_plusplus)
}
#endif