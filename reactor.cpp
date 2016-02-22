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
	EventHandlerVec ehList;
	while (loop_)
	{
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
	
}





