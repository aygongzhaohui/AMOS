/**
 * @file tsreactor.h
 * @brief
 * @author GongZhaohui
 * @version
 * @date 2016-02-24
 */

#ifndef _AMOS_TSREACTOR_H_
#define _AMOS_TSREACTOR_H_

#include "thread.h"
#include "mqreactor.h"

namespace amos
{
    class TSReactor : public MQReactor
    {
    private:
        bool IsReactorThread();

    public:
        TSReactor(ReactorImpl * impl);

        virtual int RegisterHandler(EventHandler * p,
                EvMask mask, EventHandlerCreator * creator = NULL)
        {
            if (IsReactorThread())
			{
				LOG_DEBUG("RegisterHandler run in reactor thread");
                return Reactor::RegisterHandler(p, mask, creator);
			}
            else
			{
				LOG_DEBUG("RegisterHandler run in other thread");
                return MQReactor::RegisterHandler(p, mask, creator);
			}
        }

        virtual int RemoveHandler(EventHandler * p, EvMask mask)
        {
            if (IsReactorThread())
			{
				LOG_DEBUG("RemoveHandler run in reactor thread");
                return Reactor::RemoveHandler(p, mask);
			}
            else
			{
				LOG_DEBUG("RemoveHandler run in other thread");
                return MQReactor::RemoveHandler(p, mask);
			}
        }

        virtual int SuspendHandler(EventHandler * p)
        {
            if (IsReactorThread())
			{
				LOG_DEBUG("SuspendHandler run in reactor thread");
                return Reactor::SuspendHandler(p);
			}
            else
			{
				LOG_DEBUG("SuspendHandler run in other thread");
                return MQReactor::SuspendHandler(p);
			}
        }

        virtual int ResumeHandler(EventHandler * p)
        {
            if (IsReactorThread())
			{
				LOG_DEBUG("ResumeHandler run in reactor thread");
                return Reactor::ResumeHandler(p);
			}
            else
			{
				LOG_DEBUG("ResumeHandler run in other thread");
                return MQReactor::ResumeHandler(p);
			}
        }

        virtual int TriggerHandler(EventHandler * p, EvMask mask)
        {
            if (IsReactorThread())
			{
				LOG_DEBUG("TriggerHandler run in reactor thread");
                return Reactor::TriggerHandler(p, mask);
			}
            else
			{
				LOG_DEBUG("TriggerHandler run in other thread");
                return MQReactor::TriggerHandler(p, mask);
			}
        }


        virtual TIMER RegisterTimer(EventHandler * p, MSEC delay)
        {
            if (IsReactorThread())
			{
				LOG_DEBUG("RegisterTimer run in reactor thread");
                return Reactor::RegisterTimer(p, delay);
			}
            else
			{
				LOG_DEBUG("RegisterTimer run in other thread");
                return MQReactor::RegisterTimer(p, delay);
			}
        }

        virtual int RemoveTimer(TIMER timerId)
        {
            if (IsReactorThread())
			{
				LOG_DEBUG("RemoveTimer run in reactor thread");
                return Reactor::RemoveTimer(timerId);
			}
            else
			{
				LOG_DEBUG("RemoveTimer run in other thread");
                return MQReactor::RemoveTimer(timerId);
			}
        }

        virtual int ResetTimer(TIMER timerId)
        {
            if (IsReactorThread())
			{
				LOG_DEBUG("ResetTimer run in reactor thread");
                return Reactor::ResetTimer(timerId);
			}
            else
			{
				LOG_DEBUG("ResetTimer run in other thread");
                return MQReactor::ResetTimer(timerId);
			}
        }

        virtual void RunEventLoop();

    private:
        TID thrdId_;
    };

}

#endif

