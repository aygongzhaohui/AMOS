/**
 * @file handler.h
 * @brief   
 * @author GongZhaohui
 * @version 
 * @date 2016-02-05
 */

#ifndef _BASE_HANDLER_H_
#define _BASE_HANDLER_H_

#include "basedef.h"
#include <map>

namespace amos
{

    class Reactor;
    class EventHandler
    {
    public:
        enum
        {
            READ_MASK = (1 << 0),
            WRITE_MASK = (1 << 1),
            TIMER_MASK = (1 << 5),
            ERROR_MASK = (1 << 7),
			ALL_MASK = (~0U)
        };

        class Creator
        {
        public:
            virtual EventHandler * Create()
            {
                return NULL;
            }

            virtual void Destroy(EventHandler * handler)
            {
            }
        };

    public:
        EventHandler() : reactor_(NULL)
        {
        }
        virtual ~EventHandler()
        {
            reactor_= NULL;
        }

    public:
        virtual int Open(HANDLE h = INVALID_HANDLE)
        {
            return -1;
        }

        virtual int HandleInput(HANDLE h)
        {
            return 0;
        }

        virtual int HandleOutput(HANDLE h)
        {
            return 0;
        }

        virtual int HandleClose(HANDLE h, unsigned mask)
        {
            return 0;
        }

        virtual int HandleTimeout(long timerId)
        {
            return 0;
        }

        virtual HANDLE Handle() const
        {
            return INVALID_HANDLE;
        }

        Reactor * reactor(Reactor * p = NULL)
        {
            reactor_ = p;
            return reactor_;
        }

        void FetchEvents(EvMask & r, TimerVec & tl)
        {
            tl.clear();
            r = rEvents_; rEvents_ = 0;
            tl.swap(toList_);
        }

        void SetEvents(EvMask e)
        {
            rEvents_ |= e;
        }

        EvMask REvents() const
        {
            return rEvents_;
        }

        void SetTimeout(TIMER t)
        {
            toList_.push_back(t);
        }

        void SetTimer(TIMER t)
        {
            timerSet_.insert(t);
        }

        void RMTimer(TIMER t)
        {
            timerSet_.erase(t);
        }

    private:
        Reactor * reactor_;
        EvMask rEvents_;
        TimerSet timerSet_;
        TimerVec toList_;
    };

    struct RegHandler
    {
		RegHandler() : events(0), handler(NULL), creator(NULL)
		{
		}
        RegHandler(EventHandler * h,
                EvMask e,
                EventHandler::Creator * c = NULL) :
            events(e), handler(h), creator(c)
        {
        }
        EvMask events;
        EventHandler * handler;
        EventHandler::Creator * creator;
    };
    typedef EventHandler::Creator EventHandlerCreator; 
    typedef std::map<HANDLE, RegHandler> EventHandlerMap;
    typedef EventHandlerMap::iterator EventHandlerMapIter;
    typedef EventHandlerMap::const_iterator EventHandlerMapCIter;

    typedef std::vector<EventHandler *> EventHandlerVec;
    typedef EventHandlerVec::iterator EventHandlerVecIter;

}


#endif

