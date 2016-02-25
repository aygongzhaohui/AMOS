/**
 * @file tsreactor.h
 * @brief	
 * @author GongZhaohui
 * @version 
 * @date 2016-02-24
 */

#ifndef _AMOS_TSREACTOR_H_
#define _AMOS_TSREACTOR_H_

#include "mqreactor.h"
#include "thread.h"

namespace amos
{

	class TSReactor : public MQReactor
	{
	public:
		TSReactor(ReactorImpl * impl) : MQReactor(impl);

        virtual int RegisterHandler(EventHandler * p,
				EvMask mask, EventHandlerCreator * creator = NULL);

        virtual int RemoveHandler(EventHandler * p, EvMask mask);

        virtual TIMER RegisterTimer(EventHandler * p, MSEC delay);

        virtual int RemoveTimer(TIMER timerId);

		virtual int ResetTimer(TIMER timerId);

        virtual void RunEventLoop();

	private:
		bool IsReactorThread();

	private:
		TID thrdId_;
	};



}




#endif

