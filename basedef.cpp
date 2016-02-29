/**
 * @file basedef.cpp
 * @brief
 * @author GongZhaohui
 * @version
 * @date 2016-02-28
 */

#include "basedef.h"

using namespace amos;

MSEC amos::TimeDiff(const TIMESTAMP & t1, const TIMESTAMP t2)
{
    return ((t2.tv_sec - t1.tv_sec) * 1000 + (t2.tv_nsec - t1.tv_nsec)/1000000);
}


void amos::TimeNow(TIMESTAMP & t)
{
    clock_gettime(CLOCK_MONOTONIC, &t);
}

MSEC amos::TimeDiffNow(const TIMESTAMP & past)
{
    TIMESTAMP now;
    amos::TimeNow(now);
    return amos::TimeDiff(past, now);
}

