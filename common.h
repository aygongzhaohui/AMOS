/**
 * @file common.h
 * @brief 
 * @author Jack Wong
 * @Mail: pandawkx@163.com
 * @version V1.0.0
 * @date created on : 2015-12-18 11:24:11
 * @date last modified : 2015-12-18 11:25:50
 * @copyright 2015 Jack Wong. All rights reserved.
 */

#ifndef COMMON_H_
#define COMMON_H_

inline int sendn(int fd,char *sbuf,int len)
{
    int tt_send = 0;
    int send_ret = 0;
    while((send_ret = send(fd,sbuf + tt_send,len,0))>0)
    {
        tt_send += send_ret;
        if(tt_send  == len)
            return tt_send;
    }
    return tt_send;
}

#endif
