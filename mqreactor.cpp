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
        switch (msg.mtype)
        {
        case RMSG_REGHANDLER:
            {
                EventHandler * handler = msg.handler;
                EvMask mask = (EvMask)msg.arg0.val;
                EventHandlerCreator * creator =
                    (EventHandlerCreator*)msg.arg1.ptr;
                if (Reactor::RegisterHandler(handler, mask, creator))
                {// TODO log print
                }
            }
            break;
        case RMSG_RMHANDLER:
            {
                EventHandler * handler = msg.handler;
                EvMask mask = (EvMask)msg.arg0.val;
                if (Reactor::RemoveHandler(handler, mask))
                {// TODO log print
                }
            }
            break;
        case RMSG_REGTIMER:
            {
                EventHandler * handler = msg.handler;
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
        EventHandlerVec ehList;
        //1. process all msgs in mq
        ProcessMqMsg();
        //2.poll I/O events
		PollIOEvents(ehList);
        //3.poll timer events
        timerQ_.Schedule(ehList);
        //4.handle all events
        if (ehList.size() > 0)
            HandleEvents(ehList);
    }
}


