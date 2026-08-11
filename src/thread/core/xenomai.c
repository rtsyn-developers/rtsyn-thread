/**
 * @file rtsyn/thread/xenomai.c
 * @author Sergio Hidalgo (sergiohg.dev@gmail.com)
 * @brief Compilation unit for the RTSyn thread implementation
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 * @copyright Copyright (c) Sergio Hidalgo 2026
 */

#include <pthread.h>

#include "rtsyn/thread/core.h"

int
rtsyn_thread_core_create(rtsyn_thread_t *rtsyn_thread, rtsyn_thread_attrs_t *rt_attrs)
{
    // TODO: Implement this function
}

int
rtsyn_thread_core_run(rtsyn_thread_t *rtsyn_thread, rtsyn_thread_attrs_t *rt_attrs,
                      rtsyn_thread_function_t *rt_function, void *thread_function_args)
{
    // TODO: Implement this function
}

int
rtsyn_thread_core_join(rtsyn_thread_t *rtsyn_thread, rtsyn_thread_attrs_t *rt_attrs,
                       void *return_value)
{
    // TODO: Implement this function
}

int
rtsyn_thread_core_set_priority(rtsyn_thread_t *rtsyn_thread, rtsyn_thread_attrs_t *rt_attrs,
                               int priority)
{
    // TODO: Implement this function
}

uint64_t
rtsyn_thread_core_get_time()
{
    // TODO: Implement this function
}

void
rtsyn_thread_core_sleep(uint64_t nano_secs)
{
    // TODO: Implement this function
}

void
rtsyn_thread_core_sleep_until(uint64_t abs_time_ns)
{
    // TODO: Implement this function
}
