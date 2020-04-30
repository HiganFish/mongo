//
// Created by lsmg on 4/5/20.
//


#include "EventLoop.h"
#include "Channel.h"
#include "Timer.h"

using namespace mongo;
using namespace mongo::net;

Channel::Channel(EventLoop *loop, int fd):
    loop_(loop),
    fd_(fd),
    events_(0),
    revents_(0),
    status_(ADD),
    timer_add_once_(false),
    time_over_arg(nullptr),
    timer_task_(nullptr)
{

}
Channel::~Channel()
{

}
void Channel::Update()
{
    loop_->UpdateChannel(this);
}
void Channel::HandleEvent()
{
    if (events_ & EPOLLIN)
    {
        if (read_callback_)
        {
            read_callback_();
        }
    }
    if (events_ & EPOLLOUT)
    {
        if (write_callback_)
        {
            write_callback_();
        }
    }
    if (events_ & EPOLLERR)
    {
        if (error_callback_)
        {
            error_callback_();
        }
    }
    if (events_ & EPOLLHUP)
    {
        if (close_callback_)
        {
            close_callback_();
        }
    }
}
void Channel::AddTimer(const TimeOverCallback& callback, int sec, int msec, bool repeat, int count)
{
    timer_task_.reset(new TimerTask(this, sec * 1000 + msec, 0, repeat, count));

    time_over_callback_ = callback;

    timer_add_once_ = true;
    Update();
}
void Channel::TimerOver()
{
    if (time_over_callback_)
    {
        time_over_callback_(time_over_arg);
    }
}
bool Channel::GetTimerAddOnce()
{
    if (timer_add_once_)
    {
        timer_add_once_ = false;
        return true;
    }
    return false;
}
