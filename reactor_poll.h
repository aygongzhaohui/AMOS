/**
 * @file reactor_epoll.h
 * @brief
 * @author GongZhaohui
 * @version
 * @date 2016-02-16
 */

#include "reactor_impl.h"
#include "demultiplex.h"
#include <vector>
#include <map>

#ifndef _REACTOR_POLL_H_
#define _REACTOR_POLL_H_

struct pollfd;

namespace amos
{
    class PollReactor : public ReactorImpl
    {
    public:
        PollReactor() : pollSize_(0)
        {
        }
        virtual ~PollReactor()
        {
        }

        virtual int RegisterHandle(HANDLE h, EvMask events);

        virtual int RemoveHandle(HANDLE h);

        virtual int ModifyEvents(HANDLE h, EvMask events);

        virtual int Demultiplex(EventHandlerMap & handlers,
                                RegHandlerVec & list,
                                MSEC timeout);

    private:
        typedef std::map<HANDLE, unsigned> HandleIndexMap;
        typedef HandleIndexMap::iterator HandleIndexMapIter;

    private:
        unsigned pollSize_;
        std::vector<struct pollfd> pollfds_;
        HandleIndexMap handleIndexMap_;
    };

}


#endif

