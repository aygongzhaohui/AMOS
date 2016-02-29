/**
 * @file TimerQTest.cpp
 * @brief
 * @author GongZhaohui
 * @version
 * @date 2016-02-24
 */

#include <poll.h>
#include "gtest/gtest.h"

#define private public
#define protected public

#include "timer_q.h"

using namespace amos;

unsigned long gTimerId = 0;

TEST(TimerQTest, RegsiterTimer_fail_InvalidArgs)
{
    TimerQ tq;
    RegHandler h;
    EXPECT_TRUE(tq.RegisterTimer(NULL, 10) == 0);
    EXPECT_TRUE(tq.RegisterTimer(&h, 0) == 0);
    EXPECT_TRUE(tq.RegisterTimer(&h, -1) == 0);
    EXPECT_TRUE(tq.RegisterTimer(NULL, -1) == 0);
}

TEST(TimerQTest, RegsiterTimer_CheckTimerId)
{
    TimerQ tq;
    RegHandler h;
    for (unsigned i = 1; i <= 100; ++i)
    {
        ++gTimerId;
        EXPECT_TRUE(tq.RegisterTimer(&h, 10) == gTimerId);
    }
}

TEST(TimerQTest, RegsiterTimer_CheckHandler)
{
    TimerQ tq;
    for (unsigned i = 1; i <= 100; ++i)
    {
        RegHandler h;
        EventHandler eh;
        h.handler = &eh;
        ++gTimerId;
        EXPECT_TRUE(tq.RegisterTimer(&h, 10) == gTimerId);
        EXPECT_TRUE(tq.Handler(gTimerId) == &eh);
    }
}

TEST(TimerQTest, RemoveTimer_RemoveOK)
{
    TimerQ tq;
    RegHandler h;
    EventHandler eh;
    h.handler = &eh;
    TIMER id;
    EXPECT_TRUE((id = tq.RegisterTimer(&h, 10)) > 0);
    EXPECT_TRUE(tq.RemoveTimer(id) == 0);
    EXPECT_TRUE(tq.Handler(id) == NULL);
}

TEST(TimerQTest, TimerQ_Seq)
{
    TimerQ tq;
    RegHandler h;
    EventHandler eh;
    h.handler = &eh;
    TIMER id1, id2, id3, id4, id5;
    EXPECT_TRUE((id2 = tq.RegisterTimer(&h, 20)) > 0);
    EXPECT_TRUE((id3 = tq.RegisterTimer(&h, 30)) > 0);
    EXPECT_TRUE((id1 = tq.RegisterTimer(&h, 10)) > 0);
    EXPECT_TRUE((id5 = tq.RegisterTimer(&h, 50)) > 0);
    EXPECT_TRUE((id4 = tq.RegisterTimer(&h, 40)) > 0);
    TimerQ::TimerSetIter iter = tq.tq_.begin();
    EXPECT_TRUE((*iter)->timer.Id() == id1); ++iter;
    EXPECT_TRUE((*iter)->timer.Id() == id2); ++iter;
    EXPECT_TRUE((*iter)->timer.Id() == id3); ++iter;
    EXPECT_TRUE((*iter)->timer.Id() == id4); ++iter;
    EXPECT_TRUE((*iter)->timer.Id() == id5); ++iter;
}

TEST(TimerQTest, TimerQ_ScheduleAndReset)
{
    TimerQ tq;
    RegHandler h;
    EventHandler eh;
    h.handler = &eh;
    RegHandlerVec list;
    TIMER id1, id2, id3, id4, id5;
    EXPECT_TRUE((id2 = tq.RegisterTimer(&h, 20)) > 0);
    EXPECT_TRUE((id3 = tq.RegisterTimer(&h, 30)) > 0);
    EXPECT_TRUE((id1 = tq.RegisterTimer(&h, 10)) > 0);
    EXPECT_TRUE((id5 = tq.RegisterTimer(&h, 50)) > 0);
    EXPECT_TRUE((id4 = tq.RegisterTimer(&h, 40)) > 0);
    poll(NULL, 0, 30);
    tq.Schedule(list);
    ASSERT_TRUE(list.size() == 1);
    ASSERT_TRUE(list[0]->timeout_list.size() == 3);
    EXPECT_TRUE(list[0]->timeout_list[0] == id1);
    EXPECT_TRUE(list[0]->timeout_list[1] == id2);
    EXPECT_TRUE(list[0]->timeout_list[2] == id3);
    list[0]->revents = 0;
    list[0]->timeout_list.clear();
    list.clear();
    poll(NULL, 0, 5);
    ASSERT_TRUE(tq.ResetTimer(id2) == 0);
    poll(NULL, 0, 20);
    tq.Schedule(list);
    ASSERT_TRUE(list.size() == 1);
    ASSERT_TRUE(list[0]->timeout_list.size() == 3);
    EXPECT_TRUE(list[0]->timeout_list[0] == id4);
    EXPECT_TRUE(list[0]->timeout_list[1] == id5);
    EXPECT_TRUE(list[0]->timeout_list[2] == id2);
}

TEST(TimerQTest, TimerQ_GetTimerout)
{
    TimerQ tq;
    RegHandler h;
    EventHandler eh;
    h.handler = &eh;
    RegHandlerVec list;
    TIMER id;
    EXPECT_TRUE((id = tq.RegisterTimer(&h, 20)) > 0);
    EXPECT_TRUE(tq.GetTimeout() == 20);
    EXPECT_TRUE((id = tq.RegisterTimer(&h, 15)) > 0);
    EXPECT_TRUE(tq.GetTimeout() == 15);
    EXPECT_TRUE((id = tq.RegisterTimer(&h, 10)) > 0);
    EXPECT_TRUE(tq.GetTimeout() == 10);
}



