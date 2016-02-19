/**
 * @file socketstream.h
 * @brief
 * @author GongZhaohui
 * @version
 * @date 2014-09-16
 */
#ifndef _AMOS_SOCKETSTREAM_H_
#define _AMOS_SOCKETSTREAM_H_

#include "socket.h"
#include <netinet/in.h>

namespace amos 
{

    class SocketStream : public CSocket
    {
    public:
        static const int RET_SOCK_FIN  = 0;
        static const int RET_SOCK_BUSY = -2;
        static const int RET_SOCK_DEAD = -1;

    public:
        SocketStream();

        int open(HANDLE sockfd);

        int set_tcp_nodelay(bool on);

        /**
         * @brief   
         *
         * @param   buffer
         * @param   size
         *
         * @return  >0: 发送的字节数
         *          RET_SOCK_BUSY: 中断或者缓冲区满 
         *          RET_SOCK_DEAD: 连接发生错误
         */
        int recv(char * buffer, unsigned size) const;

        /**
         * @brief   
         *
         * @param   buffer
         * @param   size
         *
         * @return  RET_SOCK_BUSY: 中断或者缓冲区满 
         *          RET_SOCK_DEAD: 连接发生错误
         */
        int send(const char * buffer, unsigned size) const;

    protected:
        struct sockaddr_in local_addr_;
        struct sockaddr_in peer_addr_;
    };

}



#endif

