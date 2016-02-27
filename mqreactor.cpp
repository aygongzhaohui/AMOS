/**
 * @file mqreactor.cpp
 * @brief
 * @author GongZhaohui
 * @version
 * @date 2016-02-23
 */

#include "mqreactor.h"

using namespace amos;

void MQReactor::ProcessMqMsg()
{
    ReactorMq tmq;
    if (!mq_.empty())
    {
        ScopeLock lock(mqlock_);
        tmq.swap(mq_);
    }
    unsigned i = 0;
    while (i < tmq.size())
    {
        ReactorMsg &msg = tmq[i];
        HANDLE h = msg.handle;
        EventHandler * handler = msg.handler;
        // avoid invalid handler ptr
        EventHandlerMapIter iter = handlerMap_.find(h);
        if ((iter != handlerMap_.end()) &&
                (iter->second.handler == handler))
        {
            continue;
        }
        switch (msg.mtype)
        {
        case RMSG_REGHANDLER:
            {
                EvMask mask = (EvMask)msg.arg0.val;
                EventHandlerCreator * creator = (EventHandlerCreator*)msg.arg1.ptr;
                if (Reactor::RegisterHandler(handler, mask, creator))
                {// TODO log print
                }
            }
            break;
        case RMSG_RMHANDLER:
            {
                EvMask mask = (EvMask)msg.arg0.val;
                if (Reactor::RemoveHandler(handler, mask))
                {// TODO log print
                }
            }
            break;
        case RMSG_SUSPEND:
            Reactor::SuspendHandler(handler);
            break;
        case RMSG_RESUME:
            Reactor::ResumeHandler(handler);
            break;
        case RMSG_REGTIMER:
            {
                TIMER id = (TIMER)msg.arg0.val;
                MSEC delay = (MSEC)msg.arg1.val;
				if (Reactor::RegisterTimer(handler, delay, id))
				{// TODO log print
				}
            }
            break;
        case RMSG_RMTIMER:
            {
                TIMER id = (TIMER)msg.arg0.val;
                if (Reactor::RemoveTimer(id))
                {// TODO log print
                }
            }
            break;
        default:
            break;
        }// end switch
        ++i;
    }// end while
}

void MQReactor::RunEventLoop()
{
    while (loop_)
    {
        //1. process all msgs in mq
        ProcessMqMsg();
        //2.poll I/O events
        PollIOEvents(evList_);
        //3.poll timer events
        timerQ_.Schedule(evList_);
        //4.handle all events
        HandleEvents(evList_);
    }
}


