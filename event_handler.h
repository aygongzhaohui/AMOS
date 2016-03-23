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
#include "refcounterT.h"
#include <map>

namespace amos
{

    class Reactor;
    class EventHandler;
    class EventHandlerCreator
    {
    public:
        virtual EventHandler * Create()
        {
            return NULL;
        }

        virtual void Delete(EventHandler * handler)
        {
        }
    };

    class EventHandler : public RefCounter<EventHandler, EventHandlerCreator>
    {
    public:
        friend class Reactor;
        enum
        {
            NONE_MASK = 0,
            READ_MASK = (1 << 0),
            WRITE_MASK = (1 << 1),
            TIMER_MASK = (1 << 5),
            ERROR_MASK = (1 << 7),
            NOIO_MASK = (1 << 8),
            ALL_MASK = (READ_MASK | WRITE_MASK | ERROR_MASK | TIMER_MASK)
        };

        typedef enum enState
        {
            NORMAL_STAT = 0,
            REMOMVED_STAT = 1,
            CLOSED_STAT = 2,
            SUSPEND_STAT = 3,
        } State;

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

        virtual void SetHandle(const HANDLE h)
        {
        }

        Reactor * reactor(Reactor * p = NULL)
        {
            reactor_ = p;
            return reactor_;
        }

    private:
        Reactor * reactor_;   // reactor attach to
    };

	class NOIOEventHandler : public EventHandler
	{
	public:
		NOIOEventHandler() : fd_(INVALID_HANDLE)
		{
		}

        virtual int Open(HANDLE h = INVALID_HANDLE);

	private:
		HANDLE fd_;
	};

    /**
     * @brief handler/handle pair which is interact between
     *        Reactor and ReactorImpl
     *     
     */
    struct RegHandler
    {
        RegHandler() :
            handler(NULL),
            state(EventHandler::NORMAL_STAT),
            events(0), revents(0)
        {
        }
        EventHandler * handler;
        EventHandler::State state;
        EvMask events;
        EvMask revents;
        TimerSet timers;
        TimerVec timeout_list;
    };
    typedef std::map<HANDLE, RegHandler> EventHandlerMap;
    typedef EventHandlerMap::iterator EventHandlerMapIter;
    typedef EventHandlerMap::const_iterator EventHandlerMapCIter;

    typedef std::vector<EventHandler *> EventHandlerVec;
    typedef EventHandlerVec::iterator EventHandlerVecIter;

    typedef std::vector<RegHandler*> RegHandlerVec;
    typedef RegHandlerVec::iterator RegHandlerVecIter;

}


#endif

