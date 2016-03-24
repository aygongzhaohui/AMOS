/**
 * @file acceptor.h
 * @brief
 * @author GongZhaohui
 * @version
 * @date 2016-02-16
 */

#ifndef _AMOS_ACCEPTOR_H_
#define _AMOS_ACCEPTOR_H_

#include "basedef.h"
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <socketstream.h>

namespace amos
{

    class Acceptor
    {
    public:
        Acceptor(const std::string & path);
        Acceptor(const std::string & ip, unsigned port);

        int Open();

        int Accept(SocketStream & ss);

		HANDLE Handle() const
		{
			return lsocket_.fd();
		}

		int ReuseAddr(bool enable)
		{
			return lsocket_.set_reuse_addr(enable);
		}

    private:
        std::string path_;
        std::string ip_;
        int port_;
		CSocket lsocket_;
        struct sockaddr_in addrin_;
        struct sockaddr_un addrun_;
        struct sockaddr * paddr_;
        unsigned addrlen_;
    };


}

#endif

