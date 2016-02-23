/**
 * @file socket.h
 * @brief
 * @author GongZhaohui
 * @version
 * @date 2014-09-16
 */
#ifndef _AMOS_SOCKET_
#define _AMOS_SOCKET_

#include "basedef.h"
#include <unistd.h>

namespace amos 
{

    class CSocket
    {
    public:
        CSocket() : fd_(-1)
        { }

        int set_linger(bool on, int ticks = 0);
        int set_nonblock(bool on);
        bool is_nonblock();
        int set_reuse_addr(bool on);
        int set_reuse_port(bool on);
        int set_send_buffer(unsigned size);
        int set_recv_buffer(unsigned size);
        int set_send_timeout(unsigned millsec);
        int set_recv_timeout(unsigned millsec);

        HANDLE fd()
        { return fd_; }

        virtual void close()
        {
            if (fd_ > 0)
            { ::close(fd_); fd_ = -1; }
        }

    protected:
        HANDLE fd_;
    };

}



#endif

