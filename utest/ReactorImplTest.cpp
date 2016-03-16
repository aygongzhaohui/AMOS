/**
 * @file ReactorImplTest.cpp
 * @brief    
 * @author GongZhaohui
 * @version 
 * @date 2016-03-09
 */
#include "gtest/gtest.h"

#define private public
#define protected public

#include "demultiplex_stub.h"
#include "reactor_epoll.h"
#include "reactor_poll.h"

using namespace amos;


TEST(EPollReactorTest, Contruct_OK)
{
	amos::ret_epoll_create1 = 100;
	amos::EPollReactor reactor;
    EXPECT_TRUE(n_epoll_create1 == 0);
}

TEST(EPollReactorTest, Contruct_FAIL)
{
    ret_epoll_create1 = -1;
    EXPECT_THROW(EPollReactor reactor, std::string);
    EXPECT_TRUE(n_epoll_create1 == 0);
}

TEST(EPollReactorTest, RegisterHandle_WrongHandle)
{
    ret_epoll_create1 = 1000;
    EPollReactor reactor;
    int ret = reactor.RegisterHandle(-1, 0);
    EXPECT_TRUE(ret == -1);
}

TEST(EPollReactorTest, RegisterHandle_FAIL)
{
    ret_epoll_create1 = 1000;
    EPollReactor reactor;
    ret_epoll_ctl = 1;
    int ret = reactor.RegisterHandle(2, EventHandler::READ_MASK | EventHandler::WRITE_MASK);
    EXPECT_TRUE(efd_epoll_ctl == 1000);
    EXPECT_TRUE(fd_epoll_ctl == 2);
    EXPECT_TRUE(op_epoll_ctl == EPOLL_CTL_ADD);
    EXPECT_TRUE(e_epoll_ctl.events == (EPOLLERR | EPOLLHUP | EPOLLIN | EPOLLOUT));
    EXPECT_TRUE(ret == -1);
}

TEST(EPollReactorTest, RegisterHandle_OK)
{
    ret_epoll_create1 = 1000;
    EPollReactor reactor;
    ret_epoll_ctl = 0;
    int ret = reactor.RegisterHandle(2, EventHandler::READ_MASK | EventHandler::WRITE_MASK);
    EXPECT_TRUE(efd_epoll_ctl == 1000);
    EXPECT_TRUE(fd_epoll_ctl == 2);
    EXPECT_TRUE(op_epoll_ctl == EPOLL_CTL_ADD);
    EXPECT_TRUE(e_epoll_ctl.events == (EPOLLERR | EPOLLHUP | EPOLLIN | EPOLLOUT));
    EXPECT_TRUE(ret == 0);
}

TEST(EPollReactorTest, RemoveHandle_OK)
{
    ret_epoll_create1 = 1000;
    EPollReactor reactor;
    ret_epoll_ctl = 0;
    int ret = reactor.RemoveHandle(2);
    EXPECT_TRUE(efd_epoll_ctl == 1000);
    EXPECT_TRUE(fd_epoll_ctl == 2);
    EXPECT_TRUE(op_epoll_ctl == EPOLL_CTL_DEL);
    EXPECT_TRUE(ret == 0);
}

TEST(EPollReactorTest, ModifyEvents_OK)
{
    ret_epoll_create1 = 1000;
    EPollReactor reactor;
    ret_epoll_ctl = 0;
    int ret = reactor.ModifyEvents(2, EventHandler::READ_MASK);
    EXPECT_TRUE(efd_epoll_ctl == 1000);
    EXPECT_TRUE(fd_epoll_ctl == 2);
    EXPECT_TRUE(op_epoll_ctl == EPOLL_CTL_MOD);
    EXPECT_TRUE(e_epoll_ctl.events == (EPOLLIN));
    EXPECT_TRUE(ret == 0);
}

class TestEventHandler : public EventHandler
{
public:
    HANDLE Handle() const
    {
        return h_;
    }

	void SetHandle(HANDLE h)
	{
		h_ = h;
	}

private:
    HANDLE h_;
};

TEST(EPollReactorTest, Demultiplex)
{
    ret_epoll_create1 = 1000;
    EPollReactor reactor;
    ret_epoll_wait = 7;
    EventHandlerMap mm;
    EventHandler handler[10];
    for (int i = 2; i < 10; ++i)
    {
        RegHandler rh;
        rh.handler = &handler[i];
		rh.handler->SetHandle(i);
        mm[i] = rh;
    }
    for (int i = 0; i < 7; i++)
    {
        struct epoll_event e;
        if (i % 2 == 0)
            e.events = EPOLLIN;
        else
            e.events = EPOLLOUT;
        e.data.fd = i + 2;
        reactor.revents_[i] = e;
    }
    RegHandlerVec rVec;
    int ret = reactor.Demultiplex(mm, rVec, 10);
    EXPECT_TRUE(ret == 7);
    EXPECT_TRUE(rVec.size() == 7);
    for (int i = 0; i < 7; ++i)
    {
        ASSERT_TRUE(rVec[i] != NULL);
        EXPECT_TRUE(rVec[i]->handler == &handler[i + 2]);
        if (i % 2 == 0)
            EXPECT_TRUE(rVec[i]->revents & EventHandler::READ_MASK); 
        else
            EXPECT_TRUE(rVec[i]->revents & EventHandler::WRITE_MASK); 
    }
}




