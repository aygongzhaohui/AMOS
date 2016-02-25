/**
 * @file reactor.cpp
 * @brief
 * @author GongZhaohui
 * @version
 * @date 2016-02-21
 */

#include "reactor.h"

using namespace amos;

Reactor::Reactor(ReactorImpl * impl)
    : impl_(impl), loop_(false)
{
    if (impl_)
        loop_ = true;
}

void Reactor::PollIOEvents(EventHandlerVec & list)
{
    MSEC nextTimeout = timerQ_.GetTimeout();
    if (nextTimeout > DEFAULT_REACT_INTERVAL)
        nextTimeout = DEFAULT_REACT_INTERVAL;
    impl_->Demultiplex(handlerMap_, list, nextTimeout);
}

void Reactor::RunEventLoop()
{
    while (loop_)
    {
        EventHandlerVec ehList;
        //1.poll I/O events
        PollIOEvents(ehList);
        //2.poll timer events
        timerQ_.Schedule(ehList);
        //3.handle all events
        if (ehList.size() > 0)
            HandleEvents(ehList);
        ehList.clear();
    }
}

void Reactor::ProcessOneHandler(EventHandler * handler)
{
    assert(handler);
    if (!handler) return;
    //1.validate handler
    EventHandlerMapIter miter = handlerMap_.find(handler->Handle());
    assert(miter != handlerMap_.end());
    assert(miter->second.handler == handler);
    if (miter == handlerMap_.end()) return;
    EventHandler::Creator * creator = miter->second.creator;
    //2.get all returned events
    EvMask ev = 0;
    TimerVec tv;
    handler->FetchREvents(ev, tv);
    if (ev == 0) return;
    //3.handle all timeout timer
    if (ev & EventHandler::TIMER_MASK)
    {
        assert(tv.size() > 0);
        if (tv.size() > 0)
        {
            TimerVecIter titer = tv.begin();
            // traverse all timeid appended
            for (; titer != tv.end(); ++titer)
            {// HandleTimeout
                TIMER id = *titer;
                int ret = handler->HandleTimeout(id);
                if (ret)
                {// need remove timer
                    timerQ_.RemoveTimer(id);
                    handler->RMTimer(id);
                }
                else ResetTimer(id);// reschedule the timer if ret==0
            }
        }
    }
    //4.handle all I/O evetns
    EvMask closeMask = 0;
    if (ev & EventHandler::READ_MASK)
    {
        int ret = handler->HandleInput(handler->Handle());
        if (ret)
            closeMask |= EventHandler::READ_MASK;
    }
    if (ev & EventHandler::WRITE_MASK)
    {
        int ret = handler->HandleOutput(handler->Handle());
        if (ret)
            closeMask |= EventHandler::WRITE_MASK;
    }
    if (ev & EventHandler::ERROR_MASK)
        closeMask |= EventHandler::ERROR_MASK;
    if (closeMask)
    {
        int ret = handler->HandleClose(handler->Handle(), closeMask);
        if (ret)
            DestroyHandler(handler, creator);
    }
}

void Reactor::HandleEvents(EventHandlerVec & list)
{
    if (list.size() > 0)
    {
        EventHandlerVecIter iter;
        for (iter = list.begin(); iter != list.end(); ++iter)
        {
            EventHandler * handler = *iter;
            ProcessOneHandler(handler);
        }
    }
}

int Reactor::RegisterHandler(EventHandler * p,
        EvMask mask, EventHandlerCreator * creator)
{
    assert(p);
    if (!p || !loop_) return -1;
    // not in handler map
    EventHandlerMapIter iter = handlerMap_.find(p->Handle());
    if (iter == handlerMap_.end())
    {
        if (impl_->RegisterHandle(p->Handle(), mask) == 0)
        {
            handlerMap_[p->Handle()] = RegHandler(p, creator);
            p->SetEvents(mask);
        }
        return 0;
    }
    // modify the registered event if is in the handler map
    RegHandler & rh = iter->second;
    assert(rh.handler == p);
    if (rh.handler != p)
    {// TODO log
        return -1;
    }
    EvMask reg = (mask ^ p->Events()) & mask;
    if (reg > 0)
    {
        p->SetEvents(p->Events() | mask);
        return impl_->ModifyEvents(p->Handle(), p->Events());
    }
    return 0;
}

/**
 * @brief 删除Handler,如果无注册事件的话
 *        handler将会被移除
 *
 * @param    p
 * @param    mask
 *
 * @return
 */
int Reactor::RemoveHandler(EventHandler * p, EvMask mask)
{
    assert(p);
    if (!p || !loop_) return -1;
    EventHandlerMapIter iter = handlerMap_.find(p->Handle());
    if (iter == handlerMap_.end()) return 0;
    RegHandler & rh = iter->second;
    assert(rh.handler == p);
    if (rh.handler != p)
    {// TODO log
        return -1;
    }
    EvMask reg = p->Events() & mask;
    if (reg > 0)
    {
        if (reg == p->Events())
        {
            handlerMap_.erase(iter);
            impl_->RemoveHandle(p->Handle());
            // clear all the timer on this handler
            TimerSet ts;
            p->ClearTimers(ts);
            TimerSetIter iter = ts.begin();
            for (; iter != ts.end(); ++iter)
                timerQ_.RemoveTimer(*iter);
            p->SetEvents(0);
            return 0;
        }
        else
        {
            if (impl_->ModifyEvents(p->Handle(), reg) == 0)
                p->SetEvents(p->Events() & (~reg));
        }
    }
    return 0;
}


int Reactor::SuspendHandler(EventHandler * p)
{
    assert(p);
    if (!p || !loop_) return -1;
    if (p->IsSuspend()) return 0;
    EventHandlerMapIter iter = handlerMap_.find(p->Handle());
    RegHandler & rh = iter->second;
    assert(rh.handler == p);
    if (rh.handler != p)
    {// TODO log
        return -1;
    }
    impl_->RemoveHandle(p->Handle());
    p->Suspend();
    return 0;
}

int Reactor::ResumeHandler(EventHandler * p)
{
    assert(p);
    if (!p || !loop_) return -1;
    if (!p->IsSuspend()) return 0;
    EventHandlerMapIter iter = handlerMap_.find(p->Handle());
    RegHandler & rh = iter->second;
    assert(rh.handler == p);
    if (rh.handler != p)
    {// TODO log
        return -1;
    }
    impl_->RegisterHandle(p->Handle(), p->Events());
    p->Resume();
    return 0;
}

TIMER Reactor::RegisterTimer(EventHandler * p, MSEC delay, TIMER id)
{
    assert(p);
    if (!p) return TimerQ::INVALID_TIMER;
    EventHandlerMapIter iter = handlerMap_.find(p->Handle());
    RegHandler & rh = iter->second;
    assert(rh.handler == p);
    if (rh.handler != p)
    {// TODO log
        return TimerQ::INVALID_TIMER;
    }
    if (loop_)
    {
        TIMER ret = id;
        if (id > 0)
            ret = timerQ_.RegisterTimer(id, p, delay);
        else
            ret = timerQ_.RegisterTimer(p, delay);
        if (ret > 0)
            p->SetTimer(ret); // add the timerid to handler
        return ret;
    }
    return TimerQ::INVALID_TIMER;
}


