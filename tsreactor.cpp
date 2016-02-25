/**
 * @file tsreactor.cpp
 * @brief	
 * @author GongZhaohui
 * @version 
 * @date 2016-02-24
 */

#include "tsreactor.h"
#include <stdio.h>
#include <stdlib.h>

using namespace amos;

static pthread_once_t gThreadOnce = PTHREAD_ONCE_INIT; 
static pthread_key_t gThreadKey;

static void DestroyThreadKey()
{
	pthread_key_delete(gThreadKey);
}

static void InitThreadKey()
{
	if (pthread_key_create(&gThreadKey, NULL))
	{
		perror("[InitThreadKey] pthread_key_create failed");
		exit(1);
	}
	atexit(DestroyThreadKey);
}

static TID GetTID()
{
	TID ret = -1;
	void * p = pthread_getspecific(gThreadKey); 
	if (!p)
	{
		ret = Thread::CurrentThread();
		pthread_setspecific(gThreadKey, (void *)ret);
	}
	ret = (long)p;
	return ret;
}

TSReactor::TSReactor(ReactorImpl * impl) 
	: MQReactor(impl), thrdId_(-1)
{
	pthread_once(&gThreadOnce, InitThreadKey);
}

bool TSReactor::IsReactorThread()
{
	assert(thrdId_ != -1);
	if (thrdId_ == -1) return false;
	return (thrdId_ == GetTID());
}

void TSReactor::RunEventLoop()
{
	assert(thrdId_ == -1);
	if (thrdId_ != -1)
	{//TODO log print
		exit(1);
	}
	thrdId_ = GetTID();
	MQReactor::RunEventLoop();
}


