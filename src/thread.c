/**
 * @file rtsyn_thread/thread.c
 * @author Sergio Hidalgo (sergiohg.dev@gmail.com)
 * @brief Compilation unit for the RT thread implementation
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 * @copyright Copyright (c) Sergio Hidalgo 2026
 */

#include <stdlib.h>
#include <string.h>

#include "rtsyn/thread.h"
#include "rtsyn/thread/core.h"

struct rtsyn_thread_e {
    rtsyn_thread_attrs_t attrs;
};

rtsyn_thread_t *
rtsyn_thread_create(int priority, size_t stack_size, int policy, int inheritsched, cpu_set_t cpuset,
                    bool use_affinity)
{
    rtsyn_thread_t *rtsyn_thread = (rtsyn_thread_t *)malloc(sizeof(rtsyn_thread_t));

    if (!rtsyn_thread)
    {
        return nullptr;
    }

    memset(&rtsyn_thread->attrs, 0, sizeof(rtsyn_thread_attrs_t));

    rtsyn_thread->attrs.priority = priority;
    rtsyn_thread->attrs.stack_size = stack_size;
    rtsyn_thread->attrs.policy = policy;
    rtsyn_thread->attrs.inheritsched = inheritsched;
    rtsyn_thread->attrs.cpuset = cpuset;
    rtsyn_thread->attrs.use_affinity = use_affinity;

    if (rtsyn_thread_core_create(rtsyn_thread, &rtsyn_thread->attrs) < 0)
    {
        free(rtsyn_thread);
        return nullptr;
    }

    return rtsyn_thread;
}

void
rtsyn_thread_destroy(rtsyn_thread_t *rtsyn_thread)
{
    rtsyn_thread_core_destroy(&rtsyn_thread->attrs);
    free(rtsyn_thread);
}

int
rtsyn_thread_run(rtsyn_thread_t *rtsyn_thread, rtsyn_thread_function_t *rt_function,
                 void *rtsyn_thread_args)
{
    return rtsyn_thread_core_run(rtsyn_thread, &rtsyn_thread->attrs, rt_function,
                                 rtsyn_thread_args);
}

int
rtsyn_thread_set_priority(rtsyn_thread_t *rtsyn_thread, int priority)
{
    return rtsyn_thread_core_set_priority(rtsyn_thread, &rtsyn_thread->attrs, priority);
}

int
rtsyn_thread_join(rtsyn_thread_t *rtsyn_thread, void *return_value)
{
    return rtsyn_thread_core_join(rtsyn_thread, &rtsyn_thread->attrs, return_value);
}

uint64_t
rtsyn_thread_get_time()
{
    return rtsyn_thread_core_get_time();
}

void
rtsyn_thread_sleep(uint64_t nano_secs)
{
    rtsyn_thread_core_sleep(nano_secs);
}

void
rtsyn_thread_sleep_until(uint64_t abs_time_ns)
{
    rtsyn_thread_core_sleep_until(abs_time_ns);
}
