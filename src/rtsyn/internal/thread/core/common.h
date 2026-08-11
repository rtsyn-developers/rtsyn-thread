/**
 * @file rtsyn/thread/core/common.h
 * @author Sergio Hidalgo (sergiohg.dev@gmail.com)
 * @brief Header for the RTSyn thread core common functions.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 * @copyright Copyright (c) Sergio Hidalgo 2026
 */
#ifndef RTSYN_INTERNAL_THREAD_CORE_COMMON_H
#define RTSYN_INTERNAL_THREAD_CORE_COMMON_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

void
rtsyn_thread_timespec_from_ns(uint64_t time_ns, struct timespec *ts);

bool
rtsyn_thread_priority_is_valid(int policy, int priority);

#endif /* RTSYN_INTERNAL_THREAD_CORE_COMMON_H */
