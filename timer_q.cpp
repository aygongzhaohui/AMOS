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

TIMER TimerQ::RegisterTimer(RegHandler * rh, MSEC delay)
{
    return RegisterTimer(AllocTimerId(), rh, delay);
}

TIMER TimerQ::RegisterTimer(TIMER timerId, RegHandler * rh, MSEC delay)
{
    if (delay <= 0 || !rh) return INVALID_TIMER;
    // set the timer object
    assert(timers_.find(timerId) == timers_.end());
    TimerWrapper & t = timers_[timerId];
    t.timer = Timer(timerId, delay);
    t.reg_handler = rh;
    tq_.insert(&t);
    rh->timers.insert(timerId);
    return timerId;
}

MSEC TimerQ::Schedule(RegHandlerVec & list)
{
    TimerSetIter iter = tq_.begin();
    while (iter != tq_.end())
    {
        Timer & t = (*iter)->timer;
        RegHandler * p = (*iter)->reg_handler;
        if (!p->handler || p->state != EventHandler::NORMAL_STAT)
        {// if suspend then ignore
            iter++; continue;
        }
        MSEC s = t.CheckTO();
        if (s <= 0)
        {// timeout, and remove from tq_
            p->timeout_list.push_back(t.Id());
            if (p->revents == 0)
            {// have not add into the timeout list of the handler
                list.push_back(p);
            }
            // Set return events
            p->revents |= EventHandler::TIMER_MASK;
            tq_.erase(iter++);
        }
        else
        {// no timeout and return
            return s;
        }
    }
    return MAX_INTTYPE_VAL(MSEC);
}






