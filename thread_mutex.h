/**
 * @file thread_mutex.h
 * @brief
 * @author GongZhaohui
 * @version
 * @date 2016-02-18
 */

#ifndef _AMOS_MUTEX_H_
#define _AMOS_MUTEX_H_

#include <errno.h>
#include <assert.h>
#include <pthread.h>
#include <stdint.h>

namespace amos
{
    /**
     * @brief   线程间的互斥量
     */
    class ThreadMutex
    {
    public:
        ThreadMutex()
        {
            m_bInitOK = false;
            if (pthread_mutex_init(&m_mutex, NULL) == 0)
                m_bInitOK = true;
            else throw 11015;
        }

        ~ThreadMutex()
        {
            pthread_mutex_destroy(&m_mutex);
        }

        /**
         * @brief  锁定互斥量, 准备访问互斥区域
         */
        void Lock()
        {
            if (m_bInitOK)
                pthread_mutex_lock(&m_mutex);
        }

        /**
         * @brief   尝试加锁
         *
         * @return  如果已经锁定, 返回1
         *          成功返回0
         */
        int32_t TryLock()
        {
            if (!m_bInitOK) return -1;
            int32_t ret = pthread_mutex_lock(&m_mutex);
            if (ret != 0)
            {
                assert(errno == EBUSY);
                return 1;
            }
            return 0;
        }

        /**
         * @brief  释放互斥量
         */
        void Unlock()
        {
            if (m_bInitOK)
                pthread_mutex_unlock(&m_mutex);
        }



    private:
        pthread_mutex_t m_mutex;
        bool m_bInitOK;
    };

    class ScopeLock
    {
    public:
        ScopeLock(ThreadMutex & m) : mutex_(m)
        {
            mutex_.Lock();
        }

        ~ScopeLock()
        {
            mutex_.Unlock();
        }

    private:
        ThreadMutex & mutex_;
    };


}

#endif



