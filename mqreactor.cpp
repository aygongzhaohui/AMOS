/**
 * @file mqreactor.cpp
 * @brief
 * @author GongZhaohui
 * @version
 * @date 2016-02-23
 */

#include "reactor.h"
#include "mqreactor.h"
#include <assert.h>

using namespace amos;


MQReactor::MQReactor(ReactorImpl * impl) :
	Reactor(impl),
	mq_(this)
{
	// register mq handler to reactor
	assert(Reactor::RegisterHandler(&mq_,
				EventHandler::READ_MASK) == 0);
}

void amos::MQReactor::ProcessMqMsg(ReactorMsgVec & v)
{
    unsigned i = 0;
    while (i < v.size())
    {
        ReactorMsg &msg = v[i];
        HANDLE h = msg.handle;
        EventHandler * handler = msg.handler;
        switch (msg.mtype)
        {
        case RMSG_REGHANDLER:
            {
                EvMask mask = (EvMask)msg.arg0.val;
                EventHandlerCreator * creator = (EventHandlerCreator*)msg.arg1.ptr;
                LOG_DEBUG("Handle Reactor msg RMSG_REGHANDLER, handle=%d, handler=0x%lx, mask=%d, creator=0x%lx",
                        h, (unsigned long)handler, mask, (unsigned long)creator);
                if (Reactor::RegisterHandler(handler, mask, creator))
                {// TODO log print
                }
            }
            break;
        case RMSG_RMHANDLER:
            {
                EvMask mask = (EvMask)msg.arg0.val;
                LOG_DEBUG("Handle Reactor msg RMSG_RMHANDLER, handle=%d, handler=0x%lx, mask=%d",
                        h, (unsigned long)handler, mask);
                if (Reactor::RemoveHandler(handler, h, mask))
                {// TODO log print
                }
            }
            break;
        case RMSG_SUSPEND:
			//LOG_DEBUG("Handle Reactor msg RMSG_SUSPEND, handle=%d, handler=0x%lx", h, (unsigned long)handler);
			Reactor::SuspendHandler(handler);
            break;
        case RMSG_RESUME:
			//LOG_DEBUG("Handle Reactor msg RMSG_RESUME, handle=%d, handler=0x%lx", h, (unsigned long)handler);
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
        //1.poll I/O events
        PollIOEvents(evList_);
        //2.poll timer events
        timerQ_.Schedule(evList_);
        //3.handle all events
        HandleEvents(evList_);
    }
}


