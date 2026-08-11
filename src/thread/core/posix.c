/**
 * @file rtsyn/thread/posix.c
 * @author Sergio Hidalgo (sergiohg.dev@gmail.com)
 * @brief Compilation unit for the RSyn thread implementation
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 * @copyright Copyright (c) Sergio Hidalgo 2026
 */

#include <errno.h>
#include <time.h>

#include "rtsyn/internal/thread/core/common.h"
#include "rtsyn/thread/core.h"

int
rtsyn_thread_core_create(rtsyn_thread_t *rtsyn_thread, rtsyn_thread_attrs_t *rt_attrs)
{

    if (!rtsyn_thread || !rt_attrs)
    {
        return -1;
    }

    if (pthread_attr_init(&rt_attrs->attr))
    {
        return -1;
    }

    if (rt_attrs->stack_size > 0)
    {
        if (pthread_attr_setstacksize(&rt_attrs->attr, rt_attrs->stack_size))
        {
            pthread_attr_destroy(&rt_attrs->attr);
            return -1;
        }
    }

    if (pthread_attr_setinheritsched(&rt_attrs->attr, rt_attrs->inheritsched))
    {
        pthread_attr_destroy(&rt_attrs->attr);
        return -1;
    }

    if (rt_attrs->use_affinity)
    {
        if (pthread_attr_setaffinity_np(&rt_attrs->attr, sizeof(cpu_set_t), &rt_attrs->cpuset))
        {
            pthread_attr_destroy(&rt_attrs->attr);
            return -1;
        }
    }

    return 0;
}

void
rtsyn_thread_core_destroy(rtsyn_thread_attrs_t *rt_attrs)
{
    pthread_attr_destroy(&rt_attrs->attr);
}

int
rtsyn_thread_core_run(rtsyn_thread_t *rtsyn_thread, rtsyn_thread_attrs_t *rt_attrs,
                      rtsyn_thread_function_t *rt_function, void *thread_function_arg)
{
    if (pthread_create(&rt_attrs->thread, &rt_attrs->attr, rt_function, thread_function_arg))
    {
        return -1;
    }
    rt_attrs->started = true;
    return 0;
}

int
rtsyn_thread_core_join(rtsyn_thread_t *rtsyn_thread, rtsyn_thread_attrs_t *rt_attrs,
                       void *return_value)
{
    if (pthread_join(rt_attrs->thread, NULL))
    {
        return -1;
    }
    rt_attrs->started = false;
    return 0;
}

int
rtsyn_thread_core_set_priority(rtsyn_thread_t *rtsyn_thread, rtsyn_thread_attrs_t *rt_attrs,
                               int priority)
{
    if (!rtsyn_thread || !rt_attrs
        || !rtsyn_thread_priority_is_valid(rt_attrs->policy, priority))
    {
        return -1;
    }

    struct sched_param param = rt_attrs->param;
    param.sched_priority = priority;

    if (rt_attrs->started)
    {
        if (pthread_setschedparam(rt_attrs->thread, rt_attrs->policy, &param))
        {
            return -1;
        }
    } else
    {
        if (pthread_attr_setschedparam(&rt_attrs->attr, &param))
        {
            return -1;
        }
    }

    rt_attrs->priority = priority;
    rt_attrs->param = param;
    return 0;
}

uint64_t
rtsyn_thread_core_get_time()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}

void
rtsyn_thread_core_sleep(uint64_t nano_secs)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    uint64_t abs_time_ns = (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec + nano_secs;

    struct timespec req;
    rtsyn_thread_timespec_from_ns(abs_time_ns, &req);

    while (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &req, NULL) == EINTR)
    {
    }
}

void
rtsyn_thread_core_sleep_until(uint64_t abs_time_ns)
{
    struct timespec req;
    rtsyn_thread_timespec_from_ns(abs_time_ns, &req);

    while (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &req, NULL) == EINTR)
    {
    }
}
