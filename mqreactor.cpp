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
                LOG_DEBUG("Handle Reactor msg RMSG_REGHANDLER, handle=%d, handler=0x%lx, mask=%d, creator=0x%lx",
                        handler->Handle(), (unsigned long)handler, mask, (unsigned long)creator);
                if (Reactor::RegisterHandler(handler, mask, creator))
                {// TODO log print
                }
            }
            break;
        case RMSG_RMHANDLER:
            {
                EvMask mask = (EvMask)msg.arg0.val;
                LOG_DEBUG("Handle Reactor msg RMSG_RMHANDLER, handle=%d, handler=0x%lx, mask=%d",
                        handler->Handle(), (unsigned long)handler, mask);
                if (Reactor::RemoveHandler(handler, mask))
                {// TODO log print
                }
            }
            break;
        case RMSG_SUSPEND:
            LOG_DEBUG("Handle Reactor msg RMSG_SUSPEND, handle=%d, handler=0x%lx",
                    handler->Handle(), (unsigned long)handler);
            Reactor::SuspendHandler(handler);
            break;
        case RMSG_RESUME:
            LOG_DEBUG("Handle Reactor msg RMSG_RESUME, handle=%d, handler=0x%lx",
                    handler->Handle(), (unsigned long)handler);
            Reactor::ResumeHandler(handler);
            break;
        case RMSG_REGTIMER:
            {
                LOG_DEBUG("Handle Reactor msg RMSG_REGTIMER, handle=%d, handler=0x%lx",
                        handler->Handle(), (unsigned long)handler);
                TIMER id = (TIMER)msg.arg0.val;
                MSEC delay = (MSEC)msg.arg1.val;
                if (Reactor::RegisterTimer(handler, delay, id))
                {// TODO log print
                }
            }
            break;
        case RMSG_RMTIMER:
            {
                LOG_DEBUG("Handle Reactor msg RMSG_RMTIMER, handle=%d, handler=0x%lx",
                        handler->Handle(), (unsigned long)handler);
                TIMER id = (TIMER)msg.arg0.val;
                if (Reactor::RemoveTimer(id))
                {// TODO log print
                }
            }
            break;
        default:
            LOG_ERROR("Handle Reactor msg, unknown msg type=%d, handle=%d, handler=0x%lx",
                    msg.mtype, handler->Handle(), (unsigned long)handler);
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


