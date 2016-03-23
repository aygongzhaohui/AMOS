/**
 * @file mqreactor.h
 * @brief
 * @author GongZhaohui
 * @version
 * @date 2016-02-24
 */

#ifndef _AMOS_MQ_REACTOR_H_
#define _AMOS_MQ_REACTOR_H_

#include "logger.h"
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
			LOG_DEBUG("Send RMSG_REGHANDLER msg to reactor mq, handle=%d, handler=0x%lx, mask=%d, creator=0x%lx",
					p->Handle(), (unsigned long)p, mask, (unsigned long)creator);
            return 0;
        }

        virtual int RemoveHandler(EventHandler * p, EvMask mask)
        {
            if (!p) return -1;
            ScopeLock lock(mqlock_);
            mq_.push_back(ReactorMsg(RMSG_RMHANDLER, p, (long)mask));
			LOG_DEBUG("Send RMSG_RMHANDLER msg to reactor mq, handle=%d, handler=0x%lx, mask=%d",
					p->Handle(), (unsigned long)p, mask);
            return 0;
        }

        virtual int SuspendHandler(EventHandler * p)
        {
            if (!p) return -1;
            ScopeLock lock(mqlock_);
            mq_.push_back(ReactorMsg(RMSG_SUSPEND, p));
			LOG_DEBUG("Send RMSG_SUSPEND msg to reactor mq, handle=%d, handler=0x%lx", p->Handle(), (unsigned long)p);
            return 0;
        }

        virtual int ResumeHandler(EventHandler * p)
        {
            if (!p) return -1;
            ScopeLock lock(mqlock_);
            mq_.push_back(ReactorMsg(RMSG_RESUME, p));
			LOG_DEBUG("Send RMSG_RESUME msg to reactor mq, handle=%d, handler=0x%lx", p->Handle(), (unsigned long)p);
            return 0;
        }

		virtual int TriggerHandler(EventHandler * p, EvMask mask)
		{
            if (!p) return -1;
            ScopeLock lock(mqlock_);
            mq_.push_back(ReactorMsg(RMSG_TRIGGER, p, mask));
			LOG_DEBUG("Send RMSG_TRIGGER msg to reactor mq, handle=%d, handler=0x%lx", p->Handle(), (unsigned long)p);
            return 0;
		}


        virtual TIMER RegisterTimer(EventHandler * p, MSEC delay)
        {
            if (!p || delay <= 0) return -1;
            TIMER timerId = timerQ_.AllocTimerId();
            ScopeLock lock(mqlock_);
            mq_.push_back(ReactorMsg(
                        RMSG_REGTIMER, p, (long)timerId, (long)delay));
			LOG_DEBUG("Send RMSG_REGTIMER msg to reactor mq, handle=%d, handler=0x%lx, timerId=%ld", p->Handle(), (unsigned long)p, timerId);
            return timerId;
        }

        virtual int RemoveTimer(TIMER timerId)
        {
            if (timerId == TimerQ::INVALID_TIMER) return -1;
            ScopeLock lock(mqlock_);
            mq_.push_back(ReactorMsg(RMSG_RMTIMER, NULL, (long)timerId));
			LOG_DEBUG("Send RMSG_RMTIMER msg to reactor mq, timerId=%ld", timerId);
            return 0;
        }

        virtual int ResetTimer(TIMER timerId)
        {
            if (timerId == TimerQ::INVALID_TIMER) return -1;
            ScopeLock lock(mqlock_);
            mq_.push_back(ReactorMsg(RMSG_RSTTIMER, NULL, (long)timerId));
			LOG_DEBUG("Send RMSG_RSTTIMER msg to reactor mq, timerId=%ld", timerId);
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


