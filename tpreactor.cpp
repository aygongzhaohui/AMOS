/**
 * @file tpreactor.cpp
 * @brief
 * @author GongZhaohui
 * @version
 * @date 2016-02-24
 */

#include "tpreactor.h"
#include "logger.h"
#include <errno.h>

using namespace amos;


TPReactor::TPReactor(ReactorImpl * impl)
    : MQReactor(impl), pos_(0)
{
    if (sem_init(&semaphore_, 0, 1) < 0)
    {//TODO log print
        loop_ = false;
		LOG_ERROR("[TPReactor::TPReactor()] sem_init failed, errno=%d", errno);
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
            //1. poll I/O events
			PollIOEvents(evList_);
			//2. Handle mq msg, try to handle
			//   maybe no msg in mq
			mq_.HandleInput(mq_.Handle());
            //3. poll timer events
            timerQ_.Schedule(evList_);
            //4.0 get one handler
			if (evList_.size() > 0)
			{
				RegHandler * rh = NULL;
				//4.1 suspend all handlers
				for (unsigned i = 0; i < evList_.size(); ++i)
				{
					rh = evList_[i];
					if (rh->handler == &mq_)
					{// exclude the mq handler
						evList_[i] = NULL; continue;
					}
					assert(rh->handler);
					Reactor::SuspendHandler(rh->handler);
				}
				//4.2 pick up a event
				rh = evList_[pos_++];
				if (!rh && pos_ < evList_.size())
					rh = evList_[pos_++];
				//4.3 give up the semphore to be slave
				sem_post(&semaphore_);
				if (rh)
				{
					assert(rh->handler);
					if (rh->handler)
					{
						//4.4 process revents of the handler
						ProcessOneHandler(*rh);
						//4.5 resume the handler
						ResumeHandler(rh->handler);
					}
				}
			}// if (evList_.size() > 0)
			else sem_post(&semaphore_);
        } //if (pos_ < evList_.size())...else
    }// while
}// end funciotn



