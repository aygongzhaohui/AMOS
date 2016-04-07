/**
 * @file reactor_mq.cpp
 * @brief	
 * @author GongZhaohui
 * @version 
 * @date 2016-03-27
 */

#include "mqreactor.h"
#include "logger.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <stdlib.h>

using namespace amos;

ReactorMq::ReactorMq(MQReactor * reactor)
{
	assert(reactor);
	reactor_ = reactor;
	if (pipe2(pipe_, O_NONBLOCK) < 0)
	{
		perror("[ReactorMq] pipe2() failed");
		exit(1);
	}
}

int ReactorMq::HandleInput(HANDLE fd)
{
	assert(fd == pipe_[0]);
	char buf[1024];
	ReactorMsgVec v;
	while (::read(fd, buf, sizeof(buf)) > 0);
	if (mq_.size() == 0) return 0;
	{
		ScopeLock lock(mqlock_);
		v.swap(mq_);
	}
	reactor_->ProcessMqMsg(v);
	return 0;
}

void ReactorMq::PushBack(const ReactorMsg & msg)
{
	ScopeLock lock(mqlock_);
	mq_.push_back(msg);
	assert(1 == ::write(pipe_[1], "W", 1));
}





