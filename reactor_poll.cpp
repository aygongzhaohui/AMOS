/**
 * @file reactor_epoll.cpp
 * @brief   
 *
 * @author GongZhaohui
 * @version 
 * @date 2016-02-16
 */

#include "reactor_poll.h"
#include "event_handler.h"
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <sys/poll.h>
#include <assert.h>

using namespace amos;

#define MAX_EPOLL_REVENTS    (1024)

int PollReactor::RegisterHandle(HANDLE h, EvMask events)
{
    if (h == INVALID_HANDLE) return -1;
	HandleIndexMapIter iter = handleIndexMap_.find(h);
	if (iter != handleIndexMap_.end()) return -1;
	// add handle
	struct pollfd d;
	d.fd = h;
	d.events = POLLHUP | POLLERR | POLLNVAL;
	if (events | EventHandler::READ_MASK) d.events |= POLLIN;
	if (events | EventHandler::WRITE_MASK) d.events |= POLLOUT;
	d.revents = 0;
	pollfds_.push_back(d);
	unsigned index = pollSize_;
	handleIndexMap_[h] = index;
    ++pollSize_;
	if (pollfds_.size() >= pollSize_)
	{
		pollfds_[index] = d;
	}
	else if (pollfds_.size() + 1 == pollSize_)
	{
		pollfds_.push_back(d);
	}
	else
	{// TODO log print
		assert(0);
	}
	return 0;
}

int PollReactor::RemoveHandle(HANDLE h)
{
    if (h == INVALID_HANDLE) return -1;
	HandleIndexMapIter iter = handleIndexMap_.find(h);
	if (iter == handleIndexMap_.end()) return -1;
	unsigned index = iter->second;
	unsigned end = pollSize_ - 1;
	// check handle
	assert(pollfds_[index].fd == h);
	if (pollfds_[index].fd != h) return -1;
	// check end pollfd
	HANDLE eh = pollfds_[end].fd;
	HandleIndexMapIter eiter = handleIndexMap_.find(eh);
	assert(eiter == handleIndexMap_.end());
	// swap the end pollfd to removed positon
	pollfds_[index] = pollfds_[end];
	// modify end pollfd's index
	eiter->second = index;
	// delete the pollfd index
	handleIndexMap_.erase(iter);
	pollSize_--;
	// delete unused fds
	if (pollfds_.size() >= (pollSize_ * 2))
	{
		std::vector<struct pollfd> tmp;
		tmp.assign(pollfds_.begin(), pollfds_.begin() + pollSize_ + 1);
		pollfds_.swap(tmp);
	}
    return 0;
}

int PollReactor::ModifyEvents(HANDLE h, EvMask events)
{
    if (h == INVALID_HANDLE) return -1;
	HandleIndexMapIter iter = handleIndexMap_.find(h);
	if (iter == handleIndexMap_.end()) return -1;
	unsigned index = iter->second;
    unsigned tevents = 0;
    if (events | EventHandler::READ_MASK) tevents |= POLLIN;
    if (events | EventHandler::WRITE_MASK) tevents |= POLLOUT;
	pollfds_[index].events = tevents;
    return 0;
}

int PollReactor::Demultiplex(const EventHandlerMap & handlers,
						EventHandlerVec & list,
						MSEC timeout)
{
	if (pollSize_ == 0) return 0;
	int ret = poll(&pollfds_[0], pollSize_, timeout);
	if (ret > 0)
	{
		for (int i = 0; i < ret; ++i)
		{
			struct pollfd & e = pollfds_[0];
			HANDLE h = e.fd;
			EventHandlerMapCIter iter = handlers.find(h);
			if (iter == handlers.end()) continue;
			EventHandler * handler = iter->second.handler;
			assert(handler);
			if (!handler)
			{// TODO log print
				continue;
			}
            if ((e.revents & POLLERR) || (e.revents & POLLNVAL)
					|| (e.revents & POLLHUP) || (!(e.revents & POLLIN)))
				handler->SetEvents(EventHandler::ERROR_MASK);
			else
			{
				if (e.revents | POLLIN)
					handler->SetEvents(EventHandler::READ_MASK);
				if (e.revents | POLLOUT)
					handler->SetEvents(EventHandler::WRITE_MASK);
			}
			list.push_back(handler);
		}
	}
    return 0;
}


