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
                EvMask mask, EventHandlerCreator * creator = NULL)
        {
            assert(p);
            if (!p || !loop_) return -1;
            // not in handler map
            EventHandlerMapIter iter = handlerMap_.find(p->Handle());
            if (iter == handlerMap_.end())
            {
                if (impl_->RegisterHandle(p->Handle(), mask) == 0)
                    handlerMap_[p->Handle()] = RegHandler(p, mask, creator);
                return 0;
            }
            // modify the registered event if is in the handler map
            RegHandler & rh = iter->second;
            assert(rh.handler == p);
            if (rh.handler != p)
            {// TODO log
                return -1;
            }
            EvMask reg = (mask ^ rh.events) & mask;
            if (reg > 0)
            {
                rh.events |= mask;
                return impl_->ModifyEvents(p->Handle(), rh.events);
            }
            return 0;
        }

        /**
         * @brief 删除Handler,如果无注册事件的话
         *        handler将会被移除
         *
         * @param    p
         * @param    mask
         *
         * @return
         */
        virtual int RemoveHandler(EventHandler * p, EvMask mask)
        {
            assert(p);
            if (!p || !loop_) return -1;
            EventHandlerMapIter iter = handlerMap_.find(p->Handle());
            RegHandler & rh = iter->second;
            assert(rh.handler == p);
            if (rh.handler != p)
            {// TODO log
                return -1;
            }
            EvMask reg = rh.events & mask;
            if (reg > 0)
            {
                if (reg == rh.events)
                {
                    handlerMap_.erase(iter);
                    impl_->RemoveHandle(p->Handle());
                    return 0;
                }
                else
                {
                    if (impl_->ModifyEvents(p->Handle(), reg) == 0)
                        rh.events &= (~reg);
                }
            }
            return 0;
        }

        virtual TIMER RegisterTimer(EventHandler * p,
                MSEC delay, TIMER id = TimerQ::INVALID_TIMER)
        {
            assert(p);
            EventHandlerMapIter iter = handlerMap_.find(p->Handle());
            RegHandler & rh = iter->second;
            assert(rh.handler == p);
            if (rh.handler != p)
            {// TODO log
                return TimerQ::INVALID_TIMER;
            }
            if (loop_)
            {
                if (id > 0)
                    return timerQ_.RegisterTimer(id, p, delay);
                else
                    return timerQ_.RegisterTimer(p, delay);
            }
            return TimerQ::INVALID_TIMER;
        }

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


