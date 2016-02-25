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
		friend class Reactor;
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
        EventHandler() : reactor_(NULL), suspend_(false)
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

		bool IsSuspend() const
		{
			return suspend_;
		}

        EvMask REvents() const
        {
            return rEvents_;
        }

		/**
		 * @brief	add the occured events
		 *          Warning:不应该暴露该接口, 用户不可以调用
		 *
		 * @param	e
		 */
        void AddREvents(EvMask e)
        {
            rEvents_ |= e;
        }

	protected:
		EvMask Events() const
		{
			return regEvents_;
		}

		void SetEvents(const EvMask mask)
		{
			regEvents_ = mask;
		}

		void Suspend()
		{
			suspend_ = true;
		}

		void Resume()
		{
			suspend_ = false;
		}

        void FetchREvents(EvMask & r, TimerVec & tl)
        {
            tl.clear();
            r = rEvents_; rEvents_ = 0;
            tl.swap(toList_);
        }

		/**
		 * @brief	Add timeout timer to toList
		 *
		 * @param	t
		 */
        void SetTimeout(TIMER t)
        {
			if (timerSet_.find(t) != timerSet_.end())
				toList_.push_back(t);
        }

		/**
		 * @brief	Add the time to timer set
		 *
		 * @param	t
		 */
        void SetTimer(TIMER t)
        {
            timerSet_.insert(t);
        }

		/**
		 * @brief	Remove timer from timer set
		 *
		 * @param	t
		 */
        void RMTimer(TIMER t)
        {
            timerSet_.erase(t);
        }

		/**
		 * @brief	Clear the timer set
		 *
		 * @param	s  return all timers cleared
		 *
		 */
		void ClearTimers(TimerSet & s)
		{
			s.swap(timerSet_);
			timerSet_.clear();
		}

    private:
		EvMask regEvents_;    // events registered
        EvMask rEvents_;      // record pending events(I/O Timer events)
        Reactor * reactor_;   // reactor attach to
        TimerSet timerSet_;   // all timers attach on the handler
        TimerVec toList_;     // timeout timer list
		bool suspend_;
    };

    struct RegHandler
    {
        RegHandler() : handler(NULL), creator(NULL)
        {
        }
        RegHandler(EventHandler * h,
                EventHandler::Creator * c = NULL) : handler(h), creator(c)
        {
        }
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

