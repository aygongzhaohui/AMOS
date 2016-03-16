/**
 * @file reactor_epoll.h
 * @brief
 * @author GongZhaohui
 * @version
 * @date 2016-02-16
 */

#include "reactor_impl.h"
#include "demultiplex.h"

#ifndef _REACTOR_EPOLL_H_
#define _REACTOR_EPOLL_H_

#define MAX_EPOLL_REVENTS    (1024)

namespace amos
{

    class EPollReactor : public ReactorImpl
    {
    public:
        EPollReactor();
        virtual ~EPollReactor();

        virtual int RegisterHandle(HANDLE h, EvMask events);

        virtual int RemoveHandle(HANDLE h);

        virtual int ModifyEvents(HANDLE h, EvMask events);

        virtual int Demultiplex(EventHandlerMap & handlers,
                                RegHandlerVec & list,
                                MSEC timeout);

    private:
        int fd_;
		struct epoll_event revents_[MAX_EPOLL_REVENTS];
    };






}


#endif

