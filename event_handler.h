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
			ERROR_MASK = (1 << 7)
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
			tl.swap(timerList_);
		}

		void SetEvents(EvMask e)
		{
			rEvents_ |= e;
		}

		void AppendTimer(TIMER t)
		{
			timerList_.push_back(t);
		}


    private:
        Reactor * reactor_;
		EvMask rEvents_;
		TimerVec timerList_;
    };

    class EventHandlerCreator
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


}


#endif

