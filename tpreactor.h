/**
 * @file tpreactor.h
 * @brief	
 * @author GongZhaohui
 * @version 
 * @date 2016-02-24
 */

#ifndef _AMOS_TPREACTOR_H_
#define _AMOS_TPREACTOR_H_

#include "mqreactor.h"
#include "thread.h"
#include "semaphore.h"

namespace amos
{

	class TPReactor : public MQReactor
	{
	public:
		TPReactor(ReactorImpl * impl);
		~TPReactor();

        virtual void RunEventLoop();

	private:
		sem_t semaphore_;
		EventHandlerVec pending_;
		unsigned pos_;
	};

}

#endif

