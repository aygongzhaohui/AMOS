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
        if (pos_ < evList_.size())
        {// be slave
            RegHandler * rh = evList_[pos_++];
            assert(rh);
			if (rh)
			{
				SuspendHandler(rh->handler);
				sem_post(&semaphore_); // give up the semphore
				ProcessOneHandler(*rh);
				ResumeHandler(rh->handler);
			}
			else
				sem_post(&semaphore_); // give up the semphore
        }
        else
        {// be master
            //0. clear the pending
			assert(pos_ == evList_.size()); 
            evList_.clear();
            pos_ = 0;
            //1 process all msgs in mq
            ProcessMqMsg();
            //2 poll I/O events
            PollIOEvents(evList_);
            //3 poll timer events
            timerQ_.Schedule(evList_);
            //3.0 get one handler
			if (evList_.size() > 0)
			{
				RegHandler * rh = evList_[pos_++];
				assert(rh->handler);
				//3.1 suspend handler
				SuspendHandler(rh->handler);
				//3.2 give up the semphore
				sem_post(&semaphore_);
				//3.3 process revents of the handler
				ProcessOneHandler(*rh);
				//3.4 resume the handler
				ResumeHandler(rh->handler);
			}
			else
				sem_post(&semaphore_);
        }
    }
}



