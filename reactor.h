/**
 * @file reactor.h
 * @brief
 *
 * @author GongZhaohui
 * @version
 * @date 2016-02-05
 */

#ifndef _AMOS_REACTOR_H_
#define _AMOS_REACTOR_H_

#include "event_handler.h"
#include "timer_q.h"
#include "reactor_impl.h"

#include <pthread.h>
#include <time.h>
#include <assert.h>

namespace amos
{

    class ReactorImpl;
    class Reactor
    {
    protected:
        static const unsigned DEFAULT_REACT_INTERVAL = 10;

    public:
        Reactor(ReactorImpl * impl);
        virtual ~Reactor()
        {
            loop_ = false;
        }

    public:
        virtual int RegisterHandler(EventHandler * p,
                EvMask mask, EventHandlerCreator * creator = NULL);

        /**
         * @brief 删除Handler,如果无注册事件的话
         *        handler将会被移除(并且删除所有相关timer)
         *
         * @param    p
         * @param    mask
         *
         * @return
         */
        virtual int RemoveHandler(EventHandler * p, EvMask mask);

        virtual TIMER RegisterTimer(EventHandler * p,
                MSEC delay, TIMER id = TimerQ::INVALID_TIMER);

        virtual int SuspendHandler(EventHandler * p);

        virtual int ResumeHandler(EventHandler * p);

        virtual int RemoveTimer(TIMER timerId)
        {
            if (loop_)
            {
                return timerQ_.RemoveTimer(timerId);
            }
            return -1;
        }

        virtual int ResetTimer(TIMER timerId)
        {
            if (loop_)
            {
                return timerQ_.ResetTimer(timerId);
            }
            return -1;
        }

        virtual void RunEventLoop();

        virtual void ExitEventLoop()
        {
            loop_ = false;
        }

    protected:
        virtual int DestroyHandler(EventHandler * p, EventHandler::Creator * creator)
        {
            if (!p) return -1;
            RemoveHandler(p, EventHandler::ALL_MASK);
            if (creator)
                creator->Destroy(p);
            return 0;
        }

        virtual void HandleEvents(EventHandlerVec & list);

        virtual void ProcessOneHandler(EventHandler * handler);

        virtual void PollIOEvents(EventHandlerVec & list);

    protected:
        ReactorImpl * impl_;
        EventHandlerMap handlerMap_;
        TimerQ timerQ_;
        bool loop_;
    };

}

#endif


