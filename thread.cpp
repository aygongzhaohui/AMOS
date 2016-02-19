/**
 * @file thread.cpp
 * @brief   
 * @author GongZhaohui
 * @version 
 * @date 2016-02-18
 */

#include <string>

#include "thread.h"

using namespace amos;

TID Thread::CurrentThread()
{
#ifdef WIN32
    return GetCurrentThreadId();
#else
    return pthread_self();
#endif
}

#ifdef WIN32
DWORD WINAPI Thread::_thread_func_(LPVOID arg)
#else
void* Thread::_thread_func_(void * arg)
#endif
{
    IRunnable * r = (IRunnable*)arg;
    if (r)
    {
        r->Run();
        Thread * p_thrd = dynamic_cast<Thread*>(r);
        if (p_thrd)
            p_thrd->b_started_ = false;
    }
    return NULL;
}

int32_t Thread::StartThread()
{
    int32_t ret = 0;
    if (b_started_)
        return -1;
    void * arg = NULL;
    if (m_pRunnable)
    {
        arg = m_pRunnable;
    }
    else
    {
        arg = this;
    }
#ifdef WIN32
    thread_ = ::CreateThread(NULL, 0, _thread_func_, arg, 0, NULL);
    if (!thread_) ret = -1;
#else
    ret = pthread_create(&thread_, NULL, _thread_func_, arg);
#endif
    if (ret)
    {
        return -2;
    }
    b_started_ = true;
    return 0;
}

void Thread::JoinThread()
{
    if (b_started_)
    {
#ifdef WIN32
        WaitForSingleObject(thread_, INFINITE);
#else
        pthread_join(thread_, NULL);
#endif
    }
}



