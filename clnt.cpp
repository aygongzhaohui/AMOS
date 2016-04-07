/**
 * @file clnt.cpp
 * @brief 
 * @author Jack Wong
 * @Mail: pandawkx@163.com
 * @version V1.0.0
 * @date created on : 2015-12-18 08:46:52
 * @date last modified : 2015-12-23 08:47:54
 * @copyright 2015 Jack Wong. All rights reserved.
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include <sys/time.h>


#include <list>

typedef std::list<struct timeval> TimerList;

TimerList timer_list;

typedef struct pollfd Pollfd;

#define MAXFDS 1
Pollfd fds[MAXFDS];

#define RECV_BUF_LEN 4096
char rbuf[RECV_BUF_LEN];

/**
 *
 *  -i server ip
 *  -p server port
 *  -l loop_times
 *  -u send unit
 *  -v print detail to stdout
 *
 */

#define DEFAULT_SVR_IP "127.0.0.1"

char svr_ip[20];
int svr_port = 6666;
int loop_n = 1000;
int send_unit = 8;
bool print_verbos = false;

double count_time_diff(struct timeval &beg,struct timeval &end)
{
    return (end.tv_sec - beg.tv_sec)*1000.0 + (double)(end.tv_usec - beg.tv_usec) / 1000.0; 
}

int main(int argc,char** argv)
{
    //set default server ip
    strcpy(svr_ip,DEFAULT_SVR_IP);
    
    // read cmd parameter
    char opt;
    const char * optString = "i:p:l:u:v";
    while((opt=getopt(argc, argv, optString))!=-1)
    {
        switch(opt)
        {
            case 'i':
                memset(svr_ip,0,sizeof(svr_ip));
                strncpy(svr_ip,optarg,sizeof(svr_ip) - 1);
                break;
            case 'p':
                svr_port = atoi(optarg);
                break;
            case 'l':
                loop_n = atoi(optarg);
                break;
            case 'u':
                send_unit = atoi(optarg);
                break;
            case 'v':
                print_verbos = true;
                break;
            case '?':
            default: 
                printf("invalid argument !!!\n");
                return -1;
        }
    }

    //print parameter
    printf("server_ip:[%s], server_port:[%d], test will loop [%d] times, send_unit:[%d] byte, print_detail:[%c]\n",
            svr_ip,svr_port,loop_n,send_unit,print_verbos?'Y':'N');
   
    //alloc send and recv buf
    char *sbuf = new char[send_unit];
    if(!sbuf)
        return -1;

    //fill send buf with ascii number
    char num_index = '0';
    for(int i =0;i<send_unit;++i)
    {
        sbuf[i] = num_index;
        if(num_index == '9')
            num_index = '0';
        else
            ++num_index;
    }

    //socket bind listen
    int sock_fd = socket(AF_INET,SOCK_STREAM,0);
    if(sock_fd < 0)
    {
        printf("create socket error\n");
        return -1;
    }

    struct sockaddr_in srv_addr;
    memset(&srv_addr,0,sizeof(struct sockaddr_in));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = inet_addr(svr_ip);
    srv_addr.sin_port = htons(svr_port);
    int socklen = sizeof(struct sockaddr_in);

    int ret = connect(sock_fd,(struct sockaddr *)(&srv_addr),socklen);

    if( ret < 0)
    {
        printf("connet error\n");
        return -1;
    }

    //put into fd

    fds[0].fd = sock_fd;
    //use poll out to start test
    fds[0].events = POLLIN | POLLOUT;
    //fds[0].events = POLLIN;

    //call pool
    printf("Poll client started ...\n");
    int tt_recv = 0;
    int recv_ok_times = 0;
    double tt_delay = 0.0;
    while(1)
    {
        int retval = poll(fds,1,-1);
        if(retval == 0)
        {
            printf("timeout !\n");
        }
        else if(retval == -1)
        {
            printf("poll error !\n");
            return -1;
        }
        else
        {
            //printf("poll return ,ready %d\n",retval);
            if(fds[0].revents & POLLIN)
            {
                //printf("POLLIN happens\n");
                int rret = recv(fds[0].fd,rbuf,RECV_BUF_LEN,0);
                if(rret == 0)
                {
                    printf("client connection closed\n");
                    close(fds[0].fd);
                    fds[0].fd = -1;
                    return -1;
                }
                else if(rret < 0)
                {
                    printf("recv error\n");
                    close(fds[0].fd);
                    fds[0].fd = -1;
                    return -1;
                }
                else
                {
                    tt_recv += rret;
                    struct timeval rtv,stv;
                    gettimeofday(&rtv,NULL);
                    while(tt_recv >= send_unit)
                    {
                        double delay = count_time_diff(timer_list.front(),rtv);
                        timer_list.pop_front();
                        tt_delay += delay;
                        if(print_verbos)
                            printf("recv ok : %d ,two-way delay %f us\n",rret,delay*1000);
                        tt_recv -= send_unit;
                        if(++recv_ok_times == loop_n)
                        {
                            //over
                            printf("test loop %d times , avg two way delay = %f us\n",loop_n,tt_delay/loop_n*1000);
                            return 0;
                        }
                        gettimeofday(&stv,NULL);
                        timer_list.push_back(stv);
                        int wret = 0;
                        if((wret = sendn(fds[0].fd,sbuf,send_unit)) != send_unit)
                        {
                            printf("send error!!!\n");
                                return -1;
                        }
                        if(print_verbos)
                            printf("send %d bytes to server\n",wret);
                    }
                }
            }
            if(fds[0].revents & POLLOUT)
            {
                //printf("POLLOUT happens\n");
                fds[0].events = POLLIN;
                int wret = 0;
                struct timeval rtv;
                gettimeofday(&rtv,NULL);
                timer_list.push_back(rtv);
                if((wret = sendn(fds[0].fd,sbuf,send_unit)) != send_unit)
                {
                    printf("send error!!!\n");
                    return -1;
                }
                if(print_verbos)
                    printf("send %d bytes to server\n",wret);
            }
            if(fds[0].revents & POLLERR)
            {
                printf("POLLERR happens\n");            
                return -1;
            }
            if(fds[0].revents & POLLHUP)
            {
                printf("POLLHUP happens\n");
                return -1;
            }
            if(fds[0].revents & POLLNVAL)
            {
                printf("POLLNVAL happens\n");            
                return -1;
            }
        } 

    }

    return 0;
}

