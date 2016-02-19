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
			return iter-second.Handler();
		}

		MSEC Schedule();

	private:
		TimerMap timers_;
		TimerSet tq_;
	};




}


#endif

