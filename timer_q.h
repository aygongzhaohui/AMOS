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

	private:
		static TIMER sNextId_ = 1; // timer id start from 1
		static const TIMER INVALID_TIMER = 0; 

	public:
		/**
		 * @brief	Allocate a timer id and add the timer
		 *          into the timer queue
		 *
		 * @param	handler
		 * @param	delay
		 *
		 * @return	> 0 timer id
		 *          0 failed
		 */
		TIMER RegisterTimer(EventHandler * handler, MSEC delay);

		int RemoveTimer(TIMER id)
		{
			TimerMapIter iter = timers_.find(id);
			if (iter == timers_.end()) return -1;
			tq_.erase(&(iter->second));
			timers_.erase(iter);

			return 0;
		}

		EventHandler * Handler(TIMER id) const
		{
			TimerMapIter iter = timers_.find(id);
			if (iter == timers_.end()) return NULL;
			return iter->second.Handler();
		}

		/**
		 * @brief	Add the timer into the timer queue
		 *          to reuse
		 *
		 * @param	id
		 */
		void RestTimer(TIMER id)
		{
			TimerMapIter iter = timers_.find(id);
			assert(iter != timers_.end());
			Timer & t = iter->second;
			if (tq_.find(&t) != tq_.end())
			{
				t.Reset();
				tq_.insert(&t);
			}
		}

		/**
		 * @brief	Check timeout timers in timer queue
		 *          and append the them to handler's timerid list,
		 *          finally remove them from timer queue
		 *
		 * @return	The remained time for next timer timeout
		 */
		MSEC Schedule(EventHandlerVec & list);

	private:
		TimerMap timers_;
		TimerSet tq_;
	};




}


#endif

