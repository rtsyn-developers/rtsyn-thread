/**
 * @file rtsyn/thread/core.h
 * @author Sergio Hidalgo (sergiohg.dev@gmail.com)
 * @brief Header for the RTSyn thread core
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 * @copyright Copyright (c) Sergio Hidalgo 2026
 */
#ifndef RTSYN_THREAD_CORE_H
#define RTSYN_THREAD_CORE_H

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>

#include "rtsyn/thread.h"

struct rtsyn_thread_attrs_e {
    pthread_t thread;
    pthread_attr_t attr;
    struct sched_param param;

    int priority;
    size_t stack_size;
    int policy;
    int inheritsched; // PTHREAD_EXPLICIT_SCHED
    cpu_set_t cpuset; // for RT isolation/affinity
    bool use_affinity;
    bool started;
};

/**
 * @brief Creates the RT communication thread
 *
 * @param rt_function The RT function to be executed by the thread
 * @param rtsyn_thread_attrs_t The thread attributes
 * @return a pointer to the result, which contains the thread if succeded
 */
int
rtsyn_thread_core_create(rtsyn_thread_t *rtsyn_thread, rtsyn_thread_attrs_t *rt_attrs);

/**
 * @brief Destroys the RT attributes (if needed any)
 *
 * @param rtsyn_thread Pointer to the thread
 */
void
rtsyn_thread_core_destroy(rtsyn_thread_attrs_t *rt_attrs);

/**
 * @brief Starts the thread
 *
 * @param rtsyn_thread Pointer to the thread
 * @param rt_function Pointer to the function
 * @param thread_function_args Pointer to the function arguments
 * @return result of the launch
 */
int
rtsyn_thread_core_run(rtsyn_thread_t *rtsyn_thread, rtsyn_thread_attrs_t *rt_attrs,
                      rtsyn_thread_function_t *rt_function, void *thread_function_args);

/**
 * @brief Updates the configured backend thread priority.
 *
 * @param rtsyn_thread Pointer to the thread
 * @param rt_attrs Pointer to the thread attributes
 * @param priority New priority
 * @return 0 if accepted, negative value otherwise
 */
int
rtsyn_thread_core_set_priority(rtsyn_thread_t *rtsyn_thread, rtsyn_thread_attrs_t *rt_attrs,
                               int priority);

/**
 * @brief Waits the thread to finish
 *
 * @param rtsyn_thread_core Pointer to the thread
 * @param rt_args Arguments to the thread
 * @param return_value Possible return value
 * @return result of the run
 */
int
rtsyn_thread_core_join(rtsyn_thread_t *rtsyn_thread, rtsyn_thread_attrs_t *rt_attrs,
                       void *return_value);

/**
 * @brief Gets time with a given backend
 *
 * @return the absolute time
 */
uint64_t
rtsyn_thread_core_get_time();

/**
 * @brief Sleeps with a given backend
 *
 */
void
rtsyn_thread_core_sleep(uint64_t nano_secs);

/**
 * @brief Sleeps until an absolute monotonic timestamp
 *
 * @param abs_time_ns Absolute CLOCK_MONOTONIC time in nanoseconds
 */
void
rtsyn_thread_core_sleep_until(uint64_t abs_time_ns);

#endif /* RTSYN_THREAD_CORE_H */
