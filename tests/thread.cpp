/**
 * @file tests/thread.cpp
 * @author Sergio Hidalgo (sergiohg.dev@gmail.com)
 * @brief Tests for the RTSyn Thread
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 * @copyright Copyright (c) Sergio Hidalgo 2026
 */

#include <gtest/gtest.h>

extern "C" {
#include "rtsyn/thread.h"
}

static void *
dummy_function(void *arg)
{
    int *value = static_cast<int *>(arg);
    *value = 42;
    return nullptr;
}

static void *
sleep_function(void *arg)
{
    uint64_t *duration_ns = static_cast<uint64_t *>(arg);
    rtsyn_thread_sleep(*duration_ns);
    return nullptr;
}

class RtsynThreadTest : public ::testing::Test {
  protected:
    cpu_set_t cpuset{};

    void
    SetUp() override
    {
        CPU_ZERO(&cpuset);
    }
};

TEST_F(RtsynThreadTest, CreateAndDestroy)
{
    rtsyn_thread_t *thread =
        rtsyn_thread_create(0, 0, SCHED_OTHER, PTHREAD_EXPLICIT_SCHED, cpuset, false);

    ASSERT_NE(thread, nullptr);
    rtsyn_thread_destroy(thread);
}

TEST_F(RtsynThreadTest, RunAndJoin)
{
    rtsyn_thread_t *thread =
        rtsyn_thread_create(0, 0, SCHED_OTHER, PTHREAD_EXPLICIT_SCHED, cpuset, false);

    ASSERT_NE(thread, nullptr);

    int result = 0;
    EXPECT_EQ(rtsyn_thread_run(thread, dummy_function, &result), 0);
    EXPECT_EQ(rtsyn_thread_join(thread, nullptr), 0);
    EXPECT_EQ(result, 42);

    rtsyn_thread_destroy(thread);
}

TEST_F(RtsynThreadTest, GetTimeReturnsNonZero)
{
    uint64_t time = rtsyn_thread_get_time();
    EXPECT_GT(time, 0u);
}

TEST_F(RtsynThreadTest, GetTimeIsMonotonic)
{
    uint64_t t1 = rtsyn_thread_get_time();
    uint64_t t2 = rtsyn_thread_get_time();
    EXPECT_GE(t2, t1);
}

TEST_F(RtsynThreadTest, SleepDuration)
{
    constexpr uint64_t sleep_ns = 10'000'000; // 10ms

    uint64_t before = rtsyn_thread_get_time();
    rtsyn_thread_sleep(sleep_ns);
    uint64_t after = rtsyn_thread_get_time();

    uint64_t elapsed = after - before;
    EXPECT_GE(elapsed, sleep_ns);
    EXPECT_LT(elapsed, sleep_ns + sleep_ns);
}

TEST_F(RtsynThreadTest, SleepUntil)
{
    constexpr uint64_t offset_ns = 10'000'000; // 10ms

    uint64_t target = rtsyn_thread_get_time() + offset_ns;
    rtsyn_thread_sleep_until(target);
    uint64_t after = rtsyn_thread_get_time();

    EXPECT_GE(after, target);
    EXPECT_LT(after, target + offset_ns);
}

TEST_F(RtsynThreadTest, ThreadWithSleep)
{
    rtsyn_thread_t *thread =
        rtsyn_thread_create(0, 0, SCHED_OTHER, PTHREAD_EXPLICIT_SCHED, cpuset, false);

    ASSERT_NE(thread, nullptr);

    uint64_t sleep_ns = 5'000'000; // 5ms
    uint64_t before = rtsyn_thread_get_time();

    EXPECT_EQ(rtsyn_thread_run(thread, sleep_function, &sleep_ns), 0);
    EXPECT_EQ(rtsyn_thread_join(thread, nullptr), 0);

    uint64_t elapsed = rtsyn_thread_get_time() - before;
    EXPECT_GE(elapsed, sleep_ns);

    rtsyn_thread_destroy(thread);
}

TEST_F(RtsynThreadTest, SetPriorityRejectsInvalidSchedOtherPriority)
{
    rtsyn_thread_t *thread =
        rtsyn_thread_create(0, 0, SCHED_OTHER, PTHREAD_EXPLICIT_SCHED, cpuset, false);

    ASSERT_NE(thread, nullptr);
    EXPECT_LT(rtsyn_thread_set_priority(thread, 1), 0);

    rtsyn_thread_destroy(thread);
}

TEST_F(RtsynThreadTest, SetPriorityAppliesValidPriorityWhileRunning)
{
    rtsyn_thread_t *thread =
        rtsyn_thread_create(0, 0, SCHED_OTHER, PTHREAD_EXPLICIT_SCHED, cpuset, false);

    ASSERT_NE(thread, nullptr);

    uint64_t sleep_ns = 20'000'000; // 20ms
    ASSERT_EQ(rtsyn_thread_run(thread, sleep_function, &sleep_ns), 0);
    EXPECT_EQ(rtsyn_thread_set_priority(thread, 0), 0);
    EXPECT_EQ(rtsyn_thread_join(thread, nullptr), 0);

    rtsyn_thread_destroy(thread);
}
