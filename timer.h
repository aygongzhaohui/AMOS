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
        Timer(TIMER timerId, MSEC d, EventHandler *h = NULL)
            : id_(timerId), delay_(d), handler_(h)
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
            return (CheckTO() < t.CheckTO());
        }

        EventHandler * Handler() const
        {
            return handler_;
        }

        TIMER Id() const
        {
            return id_;
        }

    private:
        TIMER id_;
        MSEC delay_;
        EventHandler * handler_;
        TIMESTAMP startTime_;
    };

}


#endif


