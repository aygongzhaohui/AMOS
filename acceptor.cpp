/**
 * @file acceptor.cpp
 * @brief
 *
 * @author GongZhaohui
 * @version
 * @date 2016-02-16
 */

#include "logger.h"
#include "acceptor.h"
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>

using namespace amos;

Acceptor::Acceptor(const std::string & path) : path_(path), port_(-1)
{
    assert(path_.length());
    HANDLE lfd = socket(PF_LOCAL, SOCK_STREAM, 0);
	lsocket_.open(lfd);
    addrun_.sun_family = AF_LOCAL;
    strncpy(addrun_.sun_path, path.c_str(), sizeof(addrun_.sun_path));
    paddr_ = (struct sockaddr*)&addrun_;
    addrlen_ = sizeof(addrun_);
}

Acceptor::Acceptor(const std::string & ip, unsigned port) : ip_(ip), port_(port)
{
    HANDLE lfd = socket(PF_INET, SOCK_STREAM, 0);
	lsocket_.open(lfd);
    addrin_.sin_family = AF_INET;
    addrin_.sin_port = htons(port_);
    if (ip_.length() > 0)
        addrin_.sin_addr.s_addr = inet_addr(ip.c_str());
    else
        addrin_.sin_addr.s_addr = htonl(INADDR_ANY);
    paddr_ = (struct sockaddr*)&addrin_;
    addrlen_ = sizeof(addrin_);
}

int Acceptor::Open()
{
    if (lsocket_.fd() < 0)
    {
        perror("Listen socket create failed");
        return -1;
    }
    int r = bind(lsocket_.fd(), paddr_, addrlen_);
    if (r < 0)
    {
        perror("Acceptor bind failed");
        return -1;
    }
    r = listen(lsocket_.fd(), 5);
    if (r < 0)
    {
        perror("Acceptor listen failed");
        return -1;
    }
	lsocket_.set_nonblock(true);
    return 0;
}

int Acceptor::Accept(SocketStream & ss)
{
    if (lsocket_.fd() < 0) return -1;
    HANDLE h = accept(lsocket_.fd(), NULL, NULL);
    if (h < 0) return -1;
    ss.open(h);
    return 0;
}





