/**
 * @file reactor.cpp
 * @brief	
 * @author GongZhaohui
 * @version 
 * @date 2016-02-21
 */

#include "reactor.h"

using namespace amos;

Reactor::Reactor(ReactorImpl * impl)
	: impl_(impl), loop_(false)
{
	if (impl_)
		loop_ = true;
}

void Reactor::RunEventLoop()
{
	while (loop_)
	{
		EventHandlerVec ehList;
		MSEC nextTimeout = timerQ_.Schedule(ehList);
		if (nextTimeout > DEFAULT_REACT_INTERVAL) 
			nextTimeout = DEFAULT_REACT_INTERVAL;
		int ret = impl_->Demutiplex(handlerMap_, ehList, nextTimeout); 
		if (ret > 0)
			HandleEvents(ehList);
		ehList.clear();
	}
}

void Reactor::HandleEvents(EventHandlerVec & list)
{
	if (list.size() > 0)
	{
		EventHandlerVecIter iter;
		for (iter = list.begin(); iter != list.end(); ++iter)
		{
			EventHandler * handler = *iter;
			assert(handler);
			if (!handler) continue;
			// validate handler
			EventHandlerMapIter miter = handlerMap_.find(handler->Handle());
			assert(miter != handlerMap_.end());
			assert(miter->second.handler == handler);
			if (miter == handlerMap_.end()) continue;
			EventHandler::Creator * creator = miter->second.creator;
			// get all returned events
			EvMask ev = 0;
			TimerVec tv;
			handler->FetchEvents(ev, tv);
			// handle all timeout timer
			if (ev == 0) continue;
			if (ev & EventHandler::TIMER_MASK)
			{
				assert(tv.size() > 0);
				if (tv.size() == 0) continue;
				TimerVecIter titer = tv.begin();
				// handle all timeout append on the handler
				for (; titer != tv.end(); ++titer)
				{// HandleTimeout
					TIMER id = *titer;
					int ret = handler->HandleTimeout(id);
					if (ret) RemoveTimer(id);// remove the timer
					else ResetTimer(id);// reschedule the timer
				}
			}
			EvMask closeMask = 0;
			if (ev & EventHandler::READ_MASK)
			{
				int ret = handler->HandleInput(handler->Handle());
				if (ret)
					closeMask |= EventHandler::READ_MASK;
			}
			if (ev & EventHandler::WRITE_MASK)
			{
				int ret = handler->HandleOutput(handler->Handle());
				if (ret)
					closeMask |= EventHandler::WRITE_MASK;
			}
			if (ev & EventHandler::ERROR_MASK) 
				closeMask |= EventHandler::ERROR_MASK;
			if (closeMask)
			{
				int ret = handler->HandleClose(handler->Handle(), closeMask);
				if (ret)
				{
					RemoveHandler(handler, EventHandler::ALL_MASK);
					if (creator)
						creator->Destroy(handler);
				}
			}
		} // end for (iter = list.begin(); iter != list.end(); ++iter)
	} //end if (list.size() > 0)
}





