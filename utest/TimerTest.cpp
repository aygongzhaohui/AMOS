/**
 * @file TimerTest.cpp
 * @brief UnitTest for Timer
 * @author GongZhaohui
 * @version
 * @date 2016-02-24
 */

#include <poll.h>
#include "timer.h"
#include "gtest/gtest.h"

TEST(TimerTest, Timeout_10ms_sleep10ms)
{
    amos::Timer t(1, 10);
    poll(NULL, 0, 10);
    ASSERT_TRUE(t.CheckTO() <= 0);
}

TEST(TimerTest, Timeout_10ms_sleep11ms)
{
    amos::Timer t(1, 10);
    poll(NULL, 0, 11);
    ASSERT_TRUE(t.CheckTO() < 0);
}

TEST(TimerTest, NotTimeout_10ms_sleep7ms)
{
    amos::Timer t(1, 10);
    poll(NULL, 0, 7);
    ASSERT_TRUE(t.CheckTO() >= 0);
}

TEST(TimerTest, Timeout_0ms_sleep1ms)
{
    amos::Timer t(1, 0);
    poll(NULL, 0, 1);
    ASSERT_TRUE(t.CheckTO() < 0);
}

TEST(TimerTest, Timeout_nagtive1ms)
{
    amos::Timer t(1, -1);
    ASSERT_TRUE(t.CheckTO() < 0);
}

TEST(TimerTest, TimerEqual)
{
    amos::Timer t1(1, 100);
    amos::Timer t2(1, 88);
    ASSERT_TRUE(t1 == t2);
}

TEST(TimerTest, TimerCompare)
{
    amos::Timer t1(1, 100);
    amos::Timer t2(2, 88);
    ASSERT_TRUE(t2 < t1);
}

TEST(TimerTest, TimerHanlderId)
{
    amos::Timer t(199, 100);
    ASSERT_TRUE(t.Id() == 199);
}

TEST(TimerTest, TimerReset)
{
    amos::Timer t(1, 10);
    poll(NULL, 0, 10);
    ASSERT_TRUE(t.CheckTO() <= 0);
	t.Reset();
    poll(NULL, 0, 8);
    ASSERT_TRUE(t.CheckTO() >= 0);
    poll(NULL, 0, 3);
    ASSERT_TRUE(t.CheckTO() <= 0);
}


