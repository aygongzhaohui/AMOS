/**
 * @file event_handler.cpp
 * @brief	
 * @author GongZhaohui
 * @version 
 * @date 2016-03-22
 */

#include "event_handler.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace amos;


int NOIOEventHandler::Open(HANDLE h) 
{
	if (fd_ == INVALID_HANDLE)
	{
		return ::open("/dev/null", O_RDONLY); 
	}
	return -1;
}

