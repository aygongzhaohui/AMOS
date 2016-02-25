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
		//1.poll timer events
		MSEC nextTimeout = timerQ_.Schedule(ehList);
		if (nextTimeout > DEFAULT_REACT_INTERVAL) 
			nextTimeout = DEFAULT_REACT_INTERVAL;
		//2.poll I/O events
		impl_->Demultiplex(handlerMap_, ehList, nextTimeout); 
		//3.handle all events
		if (ehList.size() > 0)
			HandleEvents(ehList);
		ehList.clear();
	}
}

void Reactor::HandleOneEvent(EventHandler * handler)
{
	assert(handler);
	if (!handler) return;
	//1.validate handler
	EventHandlerMapIter miter = handlerMap_.find(handler->Handle());
	assert(miter != handlerMap_.end());
	assert(miter->second.handler == handler);
	if (miter == handlerMap_.end()) return;
	EventHandler::Creator * creator = miter->second.creator;
	//2.get all returned events
	EvMask ev = 0;
	TimerVec tv;
	handler->FetchEvents(ev, tv);
	if (ev == 0) return;
	//3.handle all timeout timer
	if (ev & EventHandler::TIMER_MASK)
	{
		assert(tv.size() > 0);
		if (tv.size() > 0)
		{
			TimerVecIter titer = tv.begin();
			// traverse all timeid appended
			for (; titer != tv.end(); ++titer)
			{// HandleTimeout
				TIMER id = *titer;
				int ret = handler->HandleTimeout(id);
				if (ret) RemoveTimer(id);// remove the timer
				else ResetTimer(id);// reschedule the timer if ret==0
			}
		}
	}
	//4.handle all I/O evetns
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
}

void Reactor::HandleEvents(EventHandlerVec & list)
{
	if (list.size() > 0)
	{
		EventHandlerVecIter iter;
		for (iter = list.begin(); iter != list.end(); ++iter)
		{
			EventHandler * handler = *iter;
			HandleOneEvent(handler);
		}
	}
}





