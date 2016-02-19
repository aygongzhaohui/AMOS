/**
 * @file socket.cpp
 * @brief
 * @author GongZhaohui
 * @version
 * @date 2014-09-16
 */
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "socket.h"

using namespace amos;

int CSocket::set_linger(bool on, int ticks)
{
    struct linger l;
    l.l_onoff = on ? 1 : 0;
    l.l_linger = ticks;
    return setsockopt(fd_, SOL_SOCKET, SO_LINGER, &l, sizeof(l));
}

int CSocket::set_nonblock(bool on)
{
    int flag = ::fcntl(fd_, F_GETFL, 0);
    flag |= O_NONBLOCK;
    return ::fcntl(fd_, F_SETFL, flag);
}

bool CSocket::is_nonblock()
{
    int flag = ::fcntl(fd_, F_GETFL, 0);
    if (flag & O_NONBLOCK)
    {
        return true;
    }
    return false;
}

int CSocket::set_reuse_addr(bool on)
{
    int optval = on ? 1 : 0;
    return setsockopt(fd_,
            SOL_SOCKET, SO_REUSEADDR, &optval, (socklen_t)sizeof(optval));
}

int CSocket::set_reuse_port(bool on)
{
#ifdef SO_REUSEPORT
    int optval = on ? 1 : 0;
    return setsockopt(fd_,
            SOL_SOCKET, SO_REUSEPORT, &optval, (socklen_t)sizeof(optval));
#else
    if (on)
    { return -1; }
    else
    { return 0; }
#endif
}

int CSocket::set_send_buffer(unsigned size)
{
    int optval = size < 1024 ? 1024 : size;
    return setsockopt(fd_,
            SOL_SOCKET, SO_SNDBUF, &optval, (socklen_t)sizeof(optval));
}

int CSocket::set_recv_buffer(unsigned size)
{
    int optval = size < 1024 ? 1024 : size;
    return setsockopt(fd_,
            SOL_SOCKET, SO_RCVBUF, &optval, (socklen_t)sizeof(optval));
}

int CSocket::set_send_timeout(unsigned millsec)
{
    struct timeval to;
    to.tv_sec = millsec / (1000);
    to.tv_usec = (millsec % 1000) * 1000;
    return setsockopt(fd_,
            SOL_SOCKET, SO_SNDTIMEO, (char *)&to, (socklen_t)sizeof(to));
}

int CSocket::set_recv_timeout(unsigned millsec)
{
    struct timeval to;
    to.tv_sec = millsec / (1000);
    to.tv_usec = (millsec % 1000) * 1000;
    return setsockopt(fd_,
            SOL_SOCKET, SO_RCVTIMEO, (char *)&to, (socklen_t)sizeof(to));
}


