//
// Created by lsmg on 4/5/20.
//


#include "mongo/base/Logger.h"
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
    timer_task_map_(),
    timer_callback_map_()
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
void Channel::AddTimer(const TimeOverCallback& callback, const std::string& key, int sec, int msec, bool repeat, int count)
{
    if (key.empty())
    {
        LOG_ERROR << "Invalid timer key" << key;
        return;
    }

    timer_task_map_[key].reset(new TimerTask(this, key, sec * 1000 + msec, 0, repeat, count));
    timer_callback_map_[key] = callback;

    new_timer_key_ = key;
    Update();
}
void Channel::TimerOver(const std::string& key)
{

    auto callback = timer_callback_map_.find(key);
    auto task = timer_task_map_.find(key);
    if (callback == timer_callback_map_.end())
    {
        LOG_ERROR << "Unknow timer callback " << key;
        return;
    }
    if (task == timer_task_map_.end())
    {
        LOG_ERROR << "Unknow timer task " << key;
        return;
    }

    if (callback->second)
    {
        callback->second(key);
    }
}