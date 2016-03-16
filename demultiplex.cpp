/**
 * @file demultiplex.cpp
 * @brief	
 * @author GongZhaohui
 * @version 
 * @date 2016-03-09
 */

#include "demultiplex.h"

using namespace amos;


int EPoll::epoll_ctl(int efd, int op, int fd, struct epoll_event *e)
{
	return ::epoll_ctl(efd, op, fd, e);
}

int EPoll::epoll_create1(int n)
{
	return ::epoll_create1(n);
}

int EPoll::epoll_wait(int efd, struct epoll_event * events, int max, int to)
{
	return ::epoll_wait(efd, events, max, to);
}


int Poll::poll(struct pollfd * fds, int size, int to)
{
	return ::poll(fds, size, to);
}
