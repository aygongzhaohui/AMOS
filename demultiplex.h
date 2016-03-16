/**
 * @file demultiplex.h
 * @brief	
 * @author GongZhaohui
 * @version 
 * @date 2016-03-09
 */

#ifndef _AMOS_DEMULTIPLEX_H_
#define _AMOS_DEMULTIPLEX_H_

#include <sys/epoll.h>
#include <sys/poll.h>

namespace amos
{

	class EPoll
	{
	public:
		static int epoll_ctl(int efd, int op, int fd, struct epoll_event *e);

		static int epoll_create1(int n);

		static int epoll_wait(int efd, struct epoll_event * events, int max, int to); 
	};

	class Poll
	{
	public:
		static int poll(struct pollfd * fds, int size, int to);
	};

}



#endif

