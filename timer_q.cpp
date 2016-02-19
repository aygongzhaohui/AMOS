/**
 * @file timer_q.cpp
 * @brief	
 *
 * @author GongZhaohui
 * @version 
 * @date 2016-02-19
 */

#include "timer_q.h"
#include <assert.h>

using namespace amos;

TIMER TimerQ::RegisterTimer(EventHandler * handler, MSEC delay)
{
	if (delay <= 0 || !handler) return INVALID_TIMER;
	// atomic add sNextId_
	TIMER timerId = __sync_fetch_and_add(&sNextId_, 1);
	// set the timer object
	assert(timers_.find(timerId) == timers_.end());
	timers_[timerId] = Timer(timerId, delay, handler);
	return timerId;
}


MSEC TimerQ::Schedule()
{
	TimerSetIter iter = tq_.begin();
	while (iter != tq_.end())
	{
		Timer * p = *iter;
		EventHandler * handler = p->Handler();
		assert(handler);
		MSEC t = p->CheckTO();
		if (t <= 0)
		{// timeout, and remove from tq_
			handler->AppendTimer(p->Id());
			tq_.erase(iter++);
		}
		else
		{// not timeout return
			return t;
		}
	}
	return MAX_INTTYPE_VAL(MSEC);
}






