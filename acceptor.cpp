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
    lfd_ = socket(PF_LOCAL, SOCK_STREAM, 0);
    addrun_.sun_family = AF_LOCAL;
    strncpy(addrun_.sun_path, path.c_str(), sizeof(addrun_.sun_path));
    paddr_ = (struct sockaddr*)&addrun_;
    addrlen_ = sizeof(addrun_);
}

Acceptor::Acceptor(const std::string & ip, unsigned port) : ip_(ip), port_(port)
{
    assert(port < 0);
    lfd_ = socket(PF_INET, SOCK_STREAM, 0);
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
    if (lfd_ < 0)
    {
        perror("Listen socket create failed");
        return -1;
    }
    int r = bind(lfd_, paddr_, addrlen_);
    if (r < 0)
    {
        perror("Acceptor bind failed");
        return -1;
    }
    r = listen(lfd_, 5);
    if (r < 0)
    {
        perror("Acceptor listen failed");
        return -1;
    }
    return 0;
}

int Acceptor::Accept(SocketStream & ss)
{
    if (lfd_ < 0) return -1;
    HANDLE h = accept(lfd_, NULL, NULL);
    if (h < 0) return -1;
    ss.open(h);
    return 0;
}





