/**
 * @file timer_q.h
 * @brief
 * @author GongZhaohui
 * @version
 * @date 2016-02-19
 */

#ifndef _AMOS_TIMER_Q_H_
#define _AMOS_TIMER_Q_H_

#include "timer.h"
#include <map>
#include <set>
#include <assert.h>

namespace amos
{

    class TimerQ
    {
    private:
        struct TimerCompare
        {
            bool operator()(const Timer * t1, const Timer * t2)
            {
                return *t1 < *t2;
            }
        };

    private:
        typedef std::set<Timer*, TimerCompare> TimerSet;
        typedef TimerSet::iterator TimerSetIter;
        typedef std::map<TIMER, Timer> TimerMap;
        typedef TimerMap::iterator TimerMapIter;
        typedef TimerMap::const_iterator TimerMapConstIter;

    private:
        static TIMER sNextId_; // timer id start from 1

    public:
        static const TIMER INVALID_TIMER = 0;

    public:
        TIMER AllocTimerId();

        /**
         * @brief    Allocate a timer id and add the timer
         *          into the timer queue
         *
         * @param    handler
         * @param    delay
         *
         * @return    > 0 timer id
         *          0 failed
         */
        TIMER RegisterTimer(EventHandler * handler, MSEC delay);

        TIMER RegisterTimer(TIMER id, EventHandler * handler, MSEC delay);

        int RemoveTimer(TIMER id)
        {
            if (id == INVALID_TIMER) return -1;
            TimerMapIter iter = timers_.find(id);
            if (iter == timers_.end()) return -1;
            tq_.erase(&(iter->second));
            timers_.erase(iter);

            return 0;
        }

        EventHandler * Handler(TIMER id) const
        {
            if (id == INVALID_TIMER) return NULL;
            TimerMapConstIter iter = timers_.find(id);
            if (iter == timers_.end()) return NULL;
            return iter->second.Handler();
        }

        /**
         * @brief    Add the timer into the timer queue
         *          to reuse
         *
         * @param    id
         */
        int ResetTimer(TIMER id)
        {
            if (id == INVALID_TIMER) return -1;
            TimerMapIter iter = timers_.find(id);
            assert(iter != timers_.end());
            if (iter == timers_.end()) return -1;
            Timer & t = iter->second;
            if (tq_.find(&t) != tq_.end())
            {
                t.Reset();
                tq_.insert(&t);
            }
            return 0;
        }

        /**
         * @brief    Check timeout timers in timer queue
         *          and append the them to handler's timerid list,
         *          finally remove them from timer queue
         *
         * @return    The remained time for next timer timeout
         */
        MSEC Schedule(EventHandlerVec & list);

		MSEC GetTimeout() const
		{
			if (tq_.size() == 0)
				return MAX_INTTYPE_VAL(MSEC);
			return (*tq_.begin())->CheckTO();
		}

    private:
        TimerMap timers_;
        TimerSet tq_;
    };


}


#endif

