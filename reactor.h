/**
 * @file reactor.h
 * @brief   
 *
 * @author GongZhaohui
 * @version 
 * @date 2016-02-05
 */

#ifndef _BASE_REACTOR_H_
#define _BASE_REACTOR_H_

#include "event_handler.h"
#include "timer_q.h"

#include <pthread.h>
#include <time.h>
#include <assert.h>

namespace amos 
{

    struct RegHandler
    {
        RegHandler(EventHandler *h, EventHandlerCreator *c, unsigned e)
            : regEvents(e), handler(h), creator(c)
        {
        }
        EvMask mask;
        EventHandler *handler;
        EventHandlerCreator *creator;
    };

    class ReactorImpl; 
    class Reactor : public ProcessEventsListener
    {
	pubilc:
		static const unsigned DEFAULT_REACT_INTERVAL = 10;

    public:
        Reactor(ReactorImpl * impl);
        virtual ~Reactor()
		{
			loop_ = false;
		}
        
    public:
        int RegisterHandler(EventHandler * p,
				EvMask mask, EventHandlerCreator * creator = NULL)
        {
            assert(p);
            if (!p) return -1;
            EventHandlerMapIter iter = handlerMap_.find(p->Handle());
            RegHandler & rh = iter->second;
            assert(rh.handler == p);
            if (rh.handler != p)
            {// TODO log
                return -1;
            }
            if (loop_)
            {
                EvMask reg = (mask ^ rh.events) & mask;
                if (ret > 0)
                {
                    rh.events |= mask;
                    return impl_->RegisterHandle(p->Handle(), reg);
                }
            }
            return -1;
        }

        /**
         * @brief 删除Handler,如果无注册事件的话
         *        handler将会被Destroy
         *
         * @param    p
         * @param    mask
         *
         * @return    
         */
        int RemoveHandler(EventHandler * p, EvMask mask)
        {
            int ret = 0;
            assert(p);
            if (!p) return -1;
            EventHandlerMapIter iter = handlerMap_.find(p->Handle());
            RegHandler & rh = iter->second;
            assert(rh.handler == p);
            if (rh.handler != p)
            {// TODO log
                return -1;
            }
            if (loop_)
            {
                EvMask reg = rh.events & (~mask);
                ret = impl_->RemoveHandle(p, reg);
                if (ret) return -1;
                rh.events = reg;
                if (reg == 0)
                {
                    if (rh.creator)
                        rh.creator.Destroy(rh.handler);
                    handlerMap_.erase(iter);
                }
                return 0;
            }
            return -1;
        }

        TIMER RegisterTimer(EventHandler * p, MSEC delay)
        {
            assert(p);
            if (!p) return INVALID_TIMER;
            EventHandlerMapIter iter = handlerMap_.find(p->Handle());
            RegHandler & rh = iter->second;
            assert(rh.handler == p);
            if (rh.handler != p)
            {// TODO log
                return INVALID_TIMER;
            }
            if (loop_)
            {
                return timerQ_.RegisterTimer(p, delay);
            }
            retur INVALID_TIMER;
        }

        int RemoveTimer(TIMER timerId)
        {
            if (loop_)
            {
                return timerQ_.RemoveTimer(timerId);
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


    protected:
        ReactorImpl * impl_;
        EventHandlerMap handlerMap_;
        TimerQ timerQ_;
        bool loop_;
    };

}

#endif


