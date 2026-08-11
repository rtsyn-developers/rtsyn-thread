/**
 * @file rtsyn/thread/preempt_rt.c
 * @author Sergio Hidalgo (sergiohg.dev@gmail.com)
 * @brief Compilation unit for the RTSyn thread implementation
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 * @copyright Copyright (c) Sergio Hidalgo 2026
 */

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>

#include "rtsyn/internal/thread/core/common.h"
#include "rtsyn/engine/defaults.h"
#include "rtsyn/thread/core.h"

/* Keep this request active for the process lifetime. */
static int cpu_dma_latency_fd = -1;

static int
request_cpu_dma_latency(void)
{
    const char *value = getenv("RTSYN_CPU_DMA_LATENCY_US");
    if (cpu_dma_latency_fd >= 0)
    {
        return 0;
    }

    if (value && strcmp(value, "off") == 0)
    {
        return 0;
    }

    int32_t request = RTSYN_ENGINE_DEFAULT_CPU_DMA_LATENCY_US;
    if (value && value[0] != '\0')
    {
        errno = 0;
        char *end = nullptr;
        long latency_us = strtol(value, &end, 10);
        if (errno || end == value || *end != '\0' || latency_us < 0 || latency_us > INT_MAX)
        {
            fprintf(stderr,
                    "RTSyn: RTSYN_CPU_DMA_LATENCY_US must be 'off' or an integer from 0 to %d (got '%s')\n",
                    INT_MAX, value);
            return -1;
        }
        request = (int32_t)latency_us;
    }

    int fd = open("/dev/cpu_dma_latency", O_WRONLY | O_CLOEXEC);
    if (fd < 0)
    {
        fprintf(stderr, "RTSyn: cannot open /dev/cpu_dma_latency: %s\n", strerror(errno));
        return -1;
    }

    ssize_t written;
    do
    {
        written = write(fd, &request, sizeof(request));
    } while (written < 0 && errno == EINTR);

    if (written != (ssize_t)sizeof(request))
    {
        int error = written < 0 ? errno : EIO;
        fprintf(stderr, "RTSyn: cannot set CPU DMA latency request: %s\n", strerror(error));
        close(fd);
        errno = error;
        return -1;
    }

    cpu_dma_latency_fd = fd;
    fprintf(stderr, "RTSyn: CPU idle-latency QoS request active (%d us)\n", (int)request);
    return 0;
}

int
rtsyn_thread_core_create(rtsyn_thread_t *rtsyn_thread, rtsyn_thread_attrs_t *rt_attrs)
{

    if (!rtsyn_thread || !rt_attrs)
    {
        return -1;
    }

    if (!rtsyn_thread_priority_is_valid(rt_attrs->policy, rt_attrs->priority))
    {
        return -1;
    }

    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1)
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

    if (pthread_attr_setschedpolicy(&rt_attrs->attr, rt_attrs->policy))
    {
        pthread_attr_destroy(&rt_attrs->attr);
        return -1;
    }

    rt_attrs->param.sched_priority = rt_attrs->priority;

    if (pthread_attr_setschedparam(&rt_attrs->attr, &rt_attrs->param))
    {
        pthread_attr_destroy(&rt_attrs->attr);
        return -1;
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

    if (request_cpu_dma_latency() < 0)
    {
        pthread_attr_destroy(&rt_attrs->attr);
        return -1;
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
    }
    else if (pthread_attr_setschedparam(&rt_attrs->attr, &param))
    {
        return -1;
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
    int ret;
    rtsyn_thread_timespec_from_ns(abs_time_ns, &req);
    do
    {
        ret = clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &req, NULL);
    } while (ret == EINTR);
}
