/**
 * @file reactor_msg.h
 * @brief   
 * @author GongZhaohui
 * @version
 * @date 2016-02-23
 */

#ifndef _AMOS_REACTOR_MSG_H_
#define _AMOS_REACTOR_MSG_H_

#include "event_handler.h"

namespace amos
{

    typedef enum enReactorMsgType
    {
        RMSG_NONE = 0,
        RMSG_REGHANDLER = 1,
        RMSG_RMHANDLER = 2,
        RMSG_REGTIMER = 3,
		RMSG_SUSPEND = 4,
		RMSG_RESUME = 5,
        RMSG_RMTIMER = 6,
		RMSG_DESTROY = 7,
        RMSG_RSTTIMER = 8
    } ReactorMsgType;

    struct ReactorMsg
    {
        ReactorMsg(ReactorMsgType t,
                EventHandler *h, void * p = NULL)
            : mtype(t), handler(h)
        {
            arg0.ptr = p; arg1.val = 0;
        }
        ReactorMsg(ReactorMsgType t,
                EventHandler *h, long v)
            : mtype(t), handler(h)
        {
            arg0.val = v; arg1.val = 0;
        }
        ReactorMsg(ReactorMsgType t,
                EventHandler *h, long v0, void * p1)
            : mtype(t), handler(h)
        {
            arg0.val = v0; arg1.ptr = p1;
        }
        ReactorMsg(ReactorMsgType t,
                EventHandler *h, long v0, long v1)
            : mtype(t), handler(h)
        {
            arg0.val = v0; arg1.val = v1;
        }
        ReactorMsgType mtype;
        EventHandler *handler;
        union { long val; void *ptr; } arg0;
        union { long val; void *ptr; } arg1;
    };

}

#endif


