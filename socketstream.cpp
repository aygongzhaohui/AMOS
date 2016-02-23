/**
 * @file socketstream.cpp
 * @brief
 * @author GongZhaohui
 * @version
 * @date 2014-09-16
 */
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <string>

#include "socketstream.h"

using namespace amos;

SocketStream::SocketStream()
{
    memset(&local_addr_, 0, (int)sizeof(local_addr_));
    memset(&peer_addr_, 0, (int)sizeof(peer_addr_));
}

int SocketStream::open(HANDLE sockfd)
{
    if (fd_ >= 0 && fd_ != sockfd)
    {
        throw std::string("[SocketStream::open] socketstream is already open.");
    }
    else
    {
        socklen_t addrlen = (socklen_t)sizeof(struct sockaddr_in);
        if (sockfd > 0)
        {
            fd_ = sockfd;
            getsockname(fd_, (struct sockaddr*)&local_addr_, &addrlen);
            getpeername(fd_, (struct sockaddr*)&peer_addr_, &addrlen);
            return 0;
        }
    }
    return -1;
}

int SocketStream::set_tcp_nodelay(bool on)
{
    int optval = on ? 1 : 0;
    return setsockopt(fd_,
            IPPROTO_TCP, TCP_NODELAY, &optval, (socklen_t)sizeof(optval));
}

int SocketStream::recv(char * buffer, unsigned size) const
{
    errno = 0;
    int ret = ::recv(fd_, buffer, size, 0);
    if (0 == ret)
        return RET_SOCK_FIN;
    if (-1 == ret)
    {
        if (EAGAIN == errno || EINTR == errno)
        {
            return RET_SOCK_BUSY;
        }
        return RET_SOCK_DEAD;
    }
    return ret;
}

int SocketStream::send(const char * buffer, unsigned size) const
{
    errno = 0;
    int ret = ::send(fd_, buffer, size, 0);
    if (-1 == ret)
    {
        if (EWOULDBLOCK == errno || EAGAIN == errno || EINTR == errno)
            return RET_SOCK_BUSY;
        return RET_SOCK_DEAD;
    }
    return ret;
}


