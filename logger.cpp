#include <stdio.h>

#include "thread_mutex.h"


namespace amos 
{
    ThreadMutex log_lock;
    void log_print(char * pre, char * msg)
    {
        log_lock.Lock();
        printf("%s%s\n", pre, msg);
        log_lock.Unlock();
        fflush(stdout);
    }
}
