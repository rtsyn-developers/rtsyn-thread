
#include "rtsyn/internal/thread/core/common.h"

#include <sched.h>

void
rtsyn_thread_timespec_from_ns(uint64_t time_ns, struct timespec *ts)
{
    ts->tv_sec = time_ns / 1000000000ull;
    ts->tv_nsec = time_ns % 1000000000ull;
}

bool
rtsyn_thread_priority_is_valid(int policy, int priority)
{
    const int min_priority = sched_get_priority_min(policy);
    const int max_priority = sched_get_priority_max(policy);
    return min_priority >= 0 && max_priority >= 0 && priority >= min_priority
           && priority <= max_priority;
}
