/**
 * @file echo_svr.cpp
 * @brief	
 * @author GongZhaohui
 * @version 
 * @date 2016-03-23
 */

#include "acceptor.h"
#include "tpreactor.h"
#include "thread.h"
#include "reactor_epoll.h"

#include <stdio.h>
#include <stdlib.h>

using namespace amos;

class ClntHandler : public amos::EventHandler
{
public:
	HANDLE Handle()
	{
		return ss_.fd();
	}

    int HandleInput(HANDLE h)
	{
		char buf[1024];		
		int n = ss_.recv(buf, sizeof(buf));
		if (n > 0)
		{
			int sn = ss_.send(buf, n);
			if (sn == SocketStream::RET_SOCK_DEAD)
				return -1;
			return 0;
		}
		else if (n == SocketStream::RET_SOCK_BUSY)
		{
			return 0;
		}
		//printf("fd=%d recv failed\n", h);
		return -1;
	}

	int HandleClose(HANDLE h, unsigned mask)
	{
		reactor()->RemoveHandler(this, EventHandler::ALL_MASK);
		ss_.close();
		LOG_INFO("fd=%d connection closed", h);
		return 0;
	}

	amos::SocketStream & SocketStream()
	{
		return ss_;
	}

private:
	amos::SocketStream ss_;
};

class SvrHandler : public amos::EventHandler,
	               public EventHandlerCreator
{
public:
	SvrHandler(amos::Acceptor * p) : pAcceptor_(p)
	{
		assert(pAcceptor_);
	}

	HANDLE Handle()
	{
		if (pAcceptor_)
			return pAcceptor_->Handle();
		return INVALID_HANDLE;
	}

	int Open(HANDLE h = INVALID_HANDLE)
	{
		if (pAcceptor_) return pAcceptor_->Open();
		return -1;
	}

	EventHandler * Create()
	{
		return new ClntHandler;
	}

    int HandleInput(HANDLE h)
	{
		if (pAcceptor_)
		{
			ClntHandler * p = dynamic_cast<ClntHandler *>(Create());
			if (pAcceptor_->Accept(p->SocketStream()) < 0)
			{
				delete p; return -1;
			}
			if (reactor()->RegisterHandler(p, EventHandler::READ_MASK, this) < 0)
			{
				delete p; return -1;
			}
			LOG_INFO("fd=%d, Coming a new connection", h);
			return 0;
		}
		return -1;
	}

private:
	amos::Acceptor * pAcceptor_;
};

int main()
{
	std::string svrIp = "";
	unsigned svrPort = 9999;
	EPollReactor epoller;
	TPReactor reactor(&epoller);
	amos::Acceptor svrAcceptor(svrIp, svrPort);
	svrAcceptor.ReuseAddr(true);
	if (svrAcceptor.Open() < 0)
	{
		perror("svr open failed");
		exit(1);
	}
	SvrHandler svrHandler(&svrAcceptor);
	if (reactor.RegisterHandler(&svrHandler, EventHandler::READ_MASK) < 0)
	{
		perror("register svr handler failed");
		exit(2);
	}
	reactor.RunEventLoop();
	return 0;
}





