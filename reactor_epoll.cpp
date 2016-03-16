/**
 * @file reactor_epoll.cpp
 * @brief
 *
 * @author GongZhaohui
 * @version
 * @date 2016-02-16
 */

#include "reactor_epoll.h"
#include "event_handler.h"
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

using namespace amos;

EPollReactor::EPollReactor()
{
    fd_ = -1;
    fd_ = amos::EPoll::epoll_create1(0);
    if (fd_ < 0)
    {
        std::string err = "epoll_create1 failed";
        perror(err.c_str());
        throw err;
    }
}

EPollReactor::~EPollReactor()
{
    if (fd_ >= 0)
    {
        close(fd_);
    }
}

int EPollReactor::RegisterHandle(HANDLE h, EvMask events)
{
    if (h == INVALID_HANDLE) return -1;
    unsigned tevents = 0;
	tevents = EPOLLERR | EPOLLHUP;
    if (events & EventHandler::READ_MASK) tevents |= EPOLLIN;
    if (events & EventHandler::WRITE_MASK) tevents |= EPOLLOUT;
    struct epoll_event e; e.events = tevents; e.data.fd = h;
    int ret = EPoll::epoll_ctl(fd_, EPOLL_CTL_ADD, h, &e);
    if (ret)
    {
        perror("[EPollReactor::RegisterHandler] epoll_ctl failed");
        return -1;
    }
    return 0;
}

int EPollReactor::RemoveHandle(HANDLE h)
{
    if (h == INVALID_HANDLE) return -1;
    int ret = EPoll::epoll_ctl(fd_, EPOLL_CTL_DEL, h, NULL);
    if (ret)
    {
        perror("[EPollReactor::UnregisterHandler] epoll_ctl failed");
        return -1;
    }
    return 0;
}

int EPollReactor::ModifyEvents(HANDLE h, EvMask events)
{
    if (h == INVALID_HANDLE) return -1;
    unsigned tevents = 0;
    if (events & EventHandler::READ_MASK) tevents |= EPOLLIN;
    if (events & EventHandler::WRITE_MASK) tevents |= EPOLLOUT;
    struct epoll_event e; e.events = tevents; e.data.fd = h;
    int ret = EPoll::epoll_ctl(fd_, EPOLL_CTL_MOD, h, &e);
    if (ret)
    {
        perror("[EPollReactor::ModifyEvents] epoll_ctl failed");
        return -1;
    }
    return 0;
}

int EPollReactor::Demultiplex(EventHandlerMap & handlers,
                        RegHandlerVec & list,
                        MSEC timeout)
{
    int ret = EPoll::epoll_wait(fd_, revents_, MAX_EPOLL_REVENTS, timeout);
    if (ret > 0)
    {
        for (int i = 0; i < ret; ++i)
        {
            struct epoll_event & e = revents_[i];
            HANDLE h = e.data.fd;
            EventHandlerMapIter iter = handlers.find(h);
            if (iter == handlers.end()) continue;
            RegHandler & rh = iter->second;
            EventHandler * handler = rh.handler;
            if (!handler) continue;
            if (((e.events & EPOLLERR) || (e.events & EPOLLHUP)) && (!(e.events & EPOLLIN)))
                rh.revents |= EventHandler::ERROR_MASK;
            else
            {
                if (e.events & EPOLLIN)
                    rh.revents |= EventHandler::READ_MASK;
                if (e.events & EPOLLOUT)
                    rh.revents |= EventHandler::WRITE_MASK;
            }
            list.push_back(&rh);
        }
    }
    return ret;
}


