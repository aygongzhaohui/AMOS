/**
 * @file timer.h
 * @brief
 * @author GongZhaohui
 * @version
 * @date 2016-02-18
 */

#ifndef _AMOS_TIMER_H_
#define _AMOS_TIMER_H_

#include "event_handler.h"

namespace amos
{

    class Timer
    {
    public:
        Timer() : id_(0), delay_(0)
        {
        }
        Timer(TIMER timerId, MSEC d)
            : id_(timerId), delay_(d)
        {
            TimeNow(startTime_);
        }

        /**
         * @brief    Check if timer timeout
         *
         * @return    >0  remain time for timeout
         *          <=0 expired time
         */
        MSEC CheckTO() const
        {
            MSEC elapse = TimeDiffNow(startTime_);
            return (delay_ - elapse);
        }

        /**
         * @brief    Reset the start time to now
         */
        void Reset()
        {
            TimeNow(startTime_);
        }

        bool operator==(const Timer & t) const
        {
            return (id_ == t.id_);
        }

        bool operator<(const Timer & t) const
        {
            MSEC diff = TimeDiff(startTime_, t.startTime_);
            return ((delay_ - t.delay_) < diff);
        }

        TIMER Id() const
        {
            return id_;
        }

    private:
        TIMER id_;
        MSEC delay_;
        TIMESTAMP startTime_;
    };

}


#endif


