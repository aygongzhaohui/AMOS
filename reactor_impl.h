/**
 * @file reactor_impl.h
 * @brief
 * @author GongZhaohui
 * @version
 * @date 2016-02-22
 */

#ifndef _AMOS_REACTOR_IMPL_H_
#define _AMOS_REACTOR_IMPL_H_

#include "basedef.h"
#include "event_handler.h"

namespace amos
{

    class ReactorImpl
    {
    public:
        virtual int RegisterHandle(HANDLE h, EvMask reg)
        {
            return -1;
        }

        virtual int RemoveHandle(HANDLE h)
        {
            return -1;
        }

        virtual int ModifyEvents(HANDLE h, EvMask reg)
        {
            return -1;
        }

        virtual int Demultiplex(EventHandlerMap & map,
                                RegHandlerVec & list,
                                MSEC timeout)
        {
            return -1;
        }
    };




}




#endif

