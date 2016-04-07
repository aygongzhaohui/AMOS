/**
 * @file reactor_mq.h
 * @brief	
 * @author GongZhaohui
 * @version 
 * @date 2016-03-27
 */

#ifndef _AMOS_REACTOR_MQ_H_
#define _AMOS_REACTOR_MQ_H_

#include "reactor_msg.h"
#include "thread_mutex.h"
#include "reactor.h"

namespace amos
{
	class MQReactor;
	class ReactorMq : public EventHandler
	{
    protected:

	public:
		ReactorMq(MQReactor * reactor);

		int HandleInput(HANDLE fd);

		void PushBack(const ReactorMsg & msg);

		HANDLE Handle()
		{
			return pipe_[0];
		}

	protected:
		MQReactor * reactor_;
        ReactorMsgVec mq_;
        ThreadMutex mqlock_;
		HANDLE pipe_[2];
	};

}





#endif

