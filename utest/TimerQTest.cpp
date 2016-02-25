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

#include "timer.h"

TEST(TimerTest, RegsiterTimer_fail_InvalidArgs)
{
	TimerQ tq;
	EventHandler h;
	EXPECT_TRUE(tq.Register(NULL, 10) < 0);
	EXPECT_TRUE(tq.Register(&h, 0) < 0);
	EXPECT_TRUE(tq.Register(&h, -1) < 0);
	EXPECT_TRUE(tq.Register(NULL, -1) < 0);
}

TEST(TimerTest, RegsiterTimer_CheckTimerId)
{
	TimerQ tq;
	EventHandler h;
	for (int i = 1; i <= 1000; ++i)
	{
		EXPECT_TRUE(tq.Register(&h, 10) == i);
	}
}

TEST(TimerTest, RegsiterTimer_CheckHandler)
{
	TimerQ tq;
	EventHandler h;
	for (int i = 1; i <= 1000; ++i)
	{
		EXPECT_TRUE(tq.Register(&h, 10) == i);
	}

}


