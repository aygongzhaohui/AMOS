/**
 * @file thread.h
 * @brief   
 * @author GongZhaohui
 * @version 
 * @date 2016-02-18
 */

#ifndef _AMOS_THREAD_H_
#define _AMOS_THREAD_H_

#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include "thread_mutex.h"

namespace amos
{
#ifdef WIN32
    typedef DWORD   TID;
#else
    typedef pid_t   TID;
#endif

    class IRunnable
    {
    public:
        virtual void Run() = 0;
    };

    class Thread : public IRunnable
    {
    public:
        static TID CurrentThread();

    public:
        Thread(IRunnable * pRunnable = NULL) :
            b_started_(false),
            m_pRunnable(pRunnable)
        { }

        // return 0 启动成功, -1 已经启动, -2 创建线程失败
        int32_t StartThread();

        void JoinThread();

    protected:
        virtual void Run() { }

    protected:
        bool b_started_;
        IRunnable * m_pRunnable;
#ifdef WIN32
        HANDLE thread_;
#else
        pthread_t thread_;
#endif

    private:
#ifdef WIN32
        static DWORD WINAPI _thread_func_(LPVOID arg);
#else
        static void* _thread_func_(void* arg);
#endif
    };
}// namespace amos

#endif

