/**
 * @file tsreactor.h
 * @brief	
 * @author GongZhaohui
 * @version 
 * @date 2016-02-24
 */

#ifndef _AMOS_TSREACTOR_H_
#define _AMOS_TSREACTOR_H_

#include "thread.h"
#include "mqreactor.h"

namespace amos
{
	class TSReactor : public MQReactor
	{
	private:
		bool IsReactorThread();

	public:
		TSReactor(ReactorImpl * impl);

        virtual int RegisterHandler(EventHandler * p,
				EvMask mask, EventHandlerCreator * creator = NULL)
		{
			if (IsReactorThread())
				return Reactor::RegisterHandler(p, mask, creator);
			else
				return MQReactor::RegisterHandler(p, mask, creator);
		}

        virtual int RemoveHandler(EventHandler * p, EvMask mask)
		{
			if (IsReactorThread())
				return Reactor::RemoveHandler(p, mask);
			else
				return MQReactor::RemoveHandler(p, mask);
		}

        virtual TIMER RegisterTimer(EventHandler * p, MSEC delay)
		{
			if (IsReactorThread())
				return Reactor::RegisterTimer(p, delay);
			else
				return MQReactor::RegisterTimer(p, delay);
		}

        virtual int RemoveTimer(TIMER timerId)
		{
			if (IsReactorThread())
				return Reactor::RemoveTimer(timerId);
			else
				return MQReactor::RemoveTimer(timerId);
		}

		virtual int ResetTimer(TIMER timerId)
		{
			if (IsReactorThread())
				return Reactor::ResetTimer(timerId);
			else
				return MQReactor::ResetTimer(timerId);
		}

        virtual void RunEventLoop();

	private:
		TID thrdId_;
	};

}

#endif

