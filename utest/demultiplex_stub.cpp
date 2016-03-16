#include "demultiplex_stub.h"

namespace amos
{
int efd_epoll_ctl = 0;
int op_epoll_ctl = 0;
int fd_epoll_ctl = 0;
struct epoll_event e_epoll_ctl;
int ret_epoll_ctl = 0;
int n_epoll_create1 = 0;
int ret_epoll_create1 = 0;
int efd_epoll_wait = 0;
int max_epoll_wait = 0;
int to_epoll_wait = 0;
int ret_epoll_wait = 0;
struct epoll_event events_epoll_wait;

int to_poll = 0;
int size_poll = 0;
struct pollfd * fds_poll = 0;
int ret_poll = 0;
}

int amos::EPoll::epoll_ctl(int efd, int op, int fd, struct epoll_event *e)
{
	amos::efd_epoll_ctl = efd;
	amos::op_epoll_ctl = op;
	amos::fd_epoll_ctl = fd;
	if (e) amos::e_epoll_ctl = *e;
	return amos::ret_epoll_ctl;
}

int amos::EPoll::epoll_create1(int n)
{
	amos::n_epoll_create1 = n;
	return amos::ret_epoll_create1;
}

int amos::EPoll::epoll_wait(int efd, struct epoll_event * events, int max, int to)
{
	amos::efd_epoll_wait = efd;
	if (events) amos::events_epoll_wait = *events;
	amos::max_epoll_wait = max;
	amos::to_epoll_wait = to;
	return amos::ret_epoll_wait;
}

int amos::Poll::poll(struct pollfd * fds, int size, int to)
{
	amos::to_poll = to;
	amos::size_poll = size;
	amos::fds_poll = fds;
	return amos::ret_poll;
}




