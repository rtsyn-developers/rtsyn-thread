/**
 * @file rtsyn/thread.h
 * @author Sergio Hidalgo (sergiohg.dev@gmail.com)
 * @brief Header file for the RTSyn Thread.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 * @copyright Copyright (c) Sergio Hidalgo 2026
 */
#ifndef RTSYN_THREAD_H
#define RTSYN_THREAD_H

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct rtsyn_thread_e rtsyn_thread_t;
typedef struct rtsyn_thread_attrs_e rtsyn_thread_attrs_t;
typedef void *(rtsyn_thread_function_t)(void *);

/**
 * @brief Creates the RT communication thread
 *
 * @param rt_function The RT function to be executed by the thread
 * @param priority The priority of the thread
 * @return the thread if all is ok, nullptr if something fails
 */
rtsyn_thread_t *
rtsyn_thread_create(int priority, size_t stack_size, int policy, int inheritsched, cpu_set_t cpuset,
                    bool use_affinity);

/**
 * @brief Destroys the RT thread
 *
 * @param rtsyn_thread Pointer to the thread
 */
void
rtsyn_thread_destroy(rtsyn_thread_t *rtsyn_thread);

/**
 * @brief Starts the thread
 *
 * @param rtsyn_thread Pointer to the thread
 * @param rtsyn_thread_args Arguments to the thread
 * @return result of the launch
 */
int
rtsyn_thread_run(rtsyn_thread_t *rtsyn_thread, rtsyn_thread_function_t *rt_function,
                 void *thread_function_args);

/**
 * @brief Updates the configured thread priority.
 *
 * @param rtsyn_thread Pointer to the thread
 * @param priority New thread priority
 * @return 0 if accepted, negative value otherwise
 */
int
rtsyn_thread_set_priority(rtsyn_thread_t *rtsyn_thread, int priority);

/**
 * @brief Waits the thread to finish
 *
 * @param rtsyn_thread Pointer to the thread
 * @param return_value Possible return value
 * @return result of the run
 */
int
rtsyn_thread_join(rtsyn_thread_t *rtsyn_thread, void *return_value);

/**
 * @brief Gets time with a given backend
 *
 * @return the absolute time
 */
uint64_t
rtsyn_thread_get_time();

/**
 * @brief Sleeps with a given backend
 *
 */
void
rtsyn_thread_sleep(uint64_t nano_sec);

/**
 * @brief Sleeps until an absolute monotonic timestamp
 *
 * @param abs_time_ns Absolute CLOCK_MONOTONIC time in nanoseconds
 */
void
rtsyn_thread_sleep_until(uint64_t abs_time_ns);

#endif /* RTSYN_THREAD_H */
