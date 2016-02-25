/**
 * @file tpreactor.cpp
 * @brief	
 * @author GongZhaohui
 * @version 
 * @date 2016-02-24
 */

#include "tpreactor.h"

using namespace amos;


TPReactor::TPReactor(ReactorImpl * impl)
	: MQReactor(impl), pos_(0)
{
	if (sem_init(&semaphore_, 0, 1) < 0)
	{//TODO log print
		loop_ = false;
	}
}

TPReactor::~TPReactor()
{
	sem_destroy(&semaphore_);
}

void TPReactor::RunEventLoop()
{
    while (loop_)
    {
		sem_wait(&semaphore_);
		if (pos_ < pending_.size())
		{// be slave
			EventHandler * handler = pending_[pos_++];
			sem_post(&semaphore_); // give up the semphore
			ProcessOneHandler(handler);
		}
		else
		{// be master
			//0. clear the pending
			pending_.clear();
			assert(pos_ == 0); pos_ = 0;
			//1. process all msgs in mq
			ProcessMqMsg();
			//2.poll I/O events
			PollIOEvents(pending_);
			//3.poll timer events
			timerQ_.Schedule(pending_);
			EventHandler * handler = pending_[pos_++];
			sem_post(&semaphore_); // give up the semphore
			ProcessOneHandler(handler);
		}
	}
}



