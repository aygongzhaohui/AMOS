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

TIMER TimerQ::sNextId_ = 1; // timer id start from 1

TIMER TimerQ::AllocTimerId()
{
    return __sync_fetch_and_add(&sNextId_, 1);// atomic add sNextId_
}

TIMER TimerQ::RegisterTimer(EventHandler * handler, MSEC delay)
{
    return RegisterTimer(AllocTimerId(), handler, delay);
}

TIMER TimerQ::RegisterTimer(TIMER timerId, EventHandler * handler, MSEC delay)
{
    if (delay <= 0 || !handler) return INVALID_TIMER;
    // set the timer object
    assert(timers_.find(timerId) == timers_.end());
    Timer & t = timers_[timerId];
    t = Timer(timerId, delay, handler);
    tq_.insert(&t);
    return timerId;
}

MSEC TimerQ::Schedule(EventHandlerVec & list)
{
    TimerSetIter iter = tq_.begin();
    while (iter != tq_.end())
    {
        Timer * p = *iter;
        EventHandler * handler = p->Handler();
        assert(handler);
		if (!handler || hanlder->IsSuspend())
		{// if suspend then ignore
			iter++; continue;
		}
        MSEC t = p->CheckTO();
        if (t <= 0)
        {// timeout, and remove from tq_
            handler->SetTimeout(p->Id());//add to handler
            if (handler->REvents() == 0)
            {// have not add into the timeout list of the handler
                list.push_back(handler);
            }
			// Set return events
            handler->AddREvents(EventHandler::TIMER_MASK);
            tq_.erase(iter++);
        }
        else
        {// no timeout and return
            return t;
        }
    }
    return MAX_INTTYPE_VAL(MSEC);
}






