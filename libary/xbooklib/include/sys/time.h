#ifndef _SYS_TIME_H
#define _SYS_TIME_H

#include "ktime.h"
#include "types.h"

#define CLOCK_REALTIME            1 /*系统统当前时间，从1970年1.1日算起*/
#define CLOCK_MONOTONIC           2 /*系统的启动时间，不能被设置*/
#define CLOCK_PROCESS_CPUTIME_ID  3 /* 本进程运行时间*/
#define CLOCK_THREAD_CPUTIME_ID   4 /*本线程运行时间*/

#define CLOCKS_PER_SEC ((clock_t)100)   /* 1秒100个时钟 */

struct timeval {
    long tv_sec;         /* seconds */
    long tv_usec;        /* and microseconds */
};

struct timezone{ 
    int tz_minuteswest; //miniutes west of Greenwich 
    int tz_dsttime; //type of DST correction 
};

#ifndef _TIMESPEC
#define _TIMESPEC
struct timespec {
    time_t tv_sec; // seconds
    long tv_nsec; // and nanoseconds
};
#endif

int gettimeofday(struct timeval *tv, struct timezone *tz);
int clock_gettime(clockid_t clockid, struct timespec *ts);

unsigned long alarm(unsigned long second);
unsigned long ktime(ktime_t *ktm);
clock_t clock();

#endif  /* _SYS_TIME_H */
