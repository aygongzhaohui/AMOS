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

static pthread_once_t gThreadOnce = PTHREAD_ONCE_INIT; 
static pthread_key_t gThreadKey = 0;

static void InitThreadKey()
{
	if (pthread_key_create(&gThreadKey, NULL))
	{
		perror("[InitThreadKey] pthread_key_create failed");
		exit(1);
	}
}

static TID 

TSReactor::TSReactor(ReactorImpl * impl) : MQReactor(impl)
{
	pthread_once(&gThreadOnce, InitThreadKey);
}


bool TSReactor::IsReactorThread()
{
	assert(pThreadId);
	if (!pThreadId) return false;

	return 
}


