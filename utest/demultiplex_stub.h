#ifndef _UT_DEMULTIPLEX_STUB_H_
#define _UT_DEMULTIPLEX_STUB_H_


#include "demultiplex.h"


namespace amos
{

extern int efd_epoll_ctl;
extern int op_epoll_ctl;
extern int fd_epoll_ctl;
extern struct epoll_event e_epoll_ctl;
extern int ret_epoll_ctl;
extern int n_epoll_create1;
extern int ret_epoll_create1;
extern int efd_epoll_wait;
extern int max_epoll_wait;
extern int to_epoll_wait;
extern int ret_epoll_wait;
extern struct epoll_event events_epoll_wait;

extern int to_poll;
extern int size_poll;
extern struct pollfd * fds_poll;
extern int ret_poll;
}


#endif

