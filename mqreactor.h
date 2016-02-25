/**
 * @file mqreactor.h
 * @brief	
 * @author GongZhaohui
 * @version 
 * @date 2016-02-24
 */

#ifndef _AMOS_MQ_REACTOR_H_
#define _AMOS_MQ_REACTOR_H_

#include "thread_mutex.h"
#include "reactor_msg.h"
#include "reactor.h"
#include <vector>

namespace amos
{

    class MQReactor : public Reactor
    {
    protected:
        typedef std::vector<ReactorMsg> ReactorMq;

    public:
        MQReactor(ReactorImpl * impl) : Reactor(impl)
        {
        }

        virtual int RegisterHandler(EventHandler * p,
                EvMask mask, EventHandlerCreator * creator = NULL)
        {
            if (!p) return -1;
            ScopeLock lock(mqlock_);
            mq_.push_back(ReactorMsg(
                        RMSG_REGHANDLER, p, (long)mask, (void *)creator));
            return 0;
        }

        virtual int RemoveHandler(EventHandler * p, EvMask mask)
        {
            if (!p) return -1;
            ScopeLock lock(mqlock_);
            mq_.push_back(ReactorMsg(RMSG_RMHANDLER, p, (long)mask));
            return 0;
        }

		virtual int SuspendHandler(EventHandler * p)
		{
			if (!p) return -1;
            ScopeLock lock(mqlock_);
            mq_.push_back(ReactorMsg(RMSG_SUSPEND, p));
			return 0;
		}

		virtual int ResumeHandler(EventHandler * p)
		{
			if (!p) return -1;
            ScopeLock lock(mqlock_);
            mq_.push_back(ReactorMsg(RMSG_RESUME, p));
			return 0;
		}
        

        virtual TIMER RegisterTimer(EventHandler * p, MSEC delay)
        {
            if (!p || delay <= 0) return -1;
            TIMER timerId = timerQ_.AllocTimerId();
            ScopeLock lock(mqlock_);
            mq_.push_back(ReactorMsg(
                        RMSG_REGTIMER, p, (long)timerId, (long)delay));
            return timerId;
        }

        virtual int RemoveTimer(TIMER timerId)
        {
            if (timerId == TimerQ::INVALID_TIMER) return -1;
            ScopeLock lock(mqlock_);
            mq_.push_back(ReactorMsg(RMSG_RMTIMER, NULL, (long)timerId));
            return 0;
        }

		virtual int DestroyHandler(EventHandler * p, EventHandler::Creator * creator)
		{
			if (!p) return -1;
            ScopeLock lock(mqlock_);
            mq_.push_back(ReactorMsg(RMSG_DESTROY, p, creator));
			return 0;
		}

        virtual int ResetTimer(TIMER timerId)
        {
            if (timerId == TimerQ::INVALID_TIMER) return -1;
            ScopeLock lock(mqlock_);
            mq_.push_back(ReactorMsg(RMSG_RSTTIMER, NULL, (long)timerId));
            return 0;
        }

        virtual void RunEventLoop();

    protected:
        virtual void ProcessMqMsg();

    protected:
        ReactorMq mq_;
        ThreadMutex mqlock_;
    };


}


#endif


