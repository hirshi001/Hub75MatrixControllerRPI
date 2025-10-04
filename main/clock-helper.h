#pragma once

#include <time.h>
#include <errno.h>
#include <stdio.h>

#ifdef __linux__
#include <unistd.h>
#endif

// Cross-platform wrapper
int clock_nanosleep_abstime(const struct timespec *target_time)
{
#ifdef __linux__
    // Linux: call the real function
    return clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, target_time, NULL);
#else
    // macOS: emulate absolute sleep using nanosleep
    struct timespec now, ts;
    if (clock_gettime(CLOCK_REALTIME, &now) != 0) {
        perror("clock_gettime");
        return -1;
    }

    // Compute difference: ts = target_time - now
    ts.tv_sec = target_time->tv_sec - now.tv_sec;
    ts.tv_nsec = target_time->tv_nsec - now.tv_nsec;
    if (ts.tv_nsec < 0) {
        ts.tv_nsec += 1000000000;
        ts.tv_sec -= 1;
    }

    // If target time already passed, do nothing
    if (ts.tv_sec < 0) return 0;

    // Sleep for the remaining relative time
    return nanosleep(&ts, NULL);
#endif
}
