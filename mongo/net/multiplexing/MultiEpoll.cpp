//
// Created by lsmg on 4/5/20.
//

#include <unistd.h>
#include "mongo/base/Logger.h"
#include "MultiEpoll.h"
#include "mongo/net/Channel.h"

using namespace mongo;
using namespace mongo::net;

const int MultiEpoll::EVENT_INIT_NUMBERS;

MultiEpoll::MultiEpoll(EventLoop* loop):
MultiBase(loop),
epollfd_(epoll_create(5)),
events_(EVENT_INIT_NUMBERS)
{
    LOG_FATAL_IF(epollfd_ == -1) << "epoll create";
}

MultiEpoll::~MultiEpoll()
{
    close(epollfd_);
}

void MultiEpoll::UpdateChannel(Channel* channel)
{
    switch (channel->GetStatus())
    {
    case Channel::ADD:
    {
        Update(EPOLL_CTL_ADD, channel);
        channel->SetStatus(Channel::ADDED);
        break;
    }
    case Channel::ADDED:
    {
        if (channel->GetREvents() == 0)
        {
            Update(EPOLL_CTL_DEL, channel);
            channel->SetStatus(Channel::DELED);
        }
        else
        {
            Update(EPOLL_CTL_MOD, channel);
        }
        break;
    }
    case Channel::DELED:
        break;
    }
}

bool MultiEpoll::LoopOnceWithTimeout(int msec, ActiveChannelList* channel_list)
{
    int number = epoll_wait(epollfd_, events_.data(), static_cast<int>(events_.size()), msec);
    if (number < 0)
    {
        LOG_FATAL_IF(errno != EINTR) << "epoll_wait error";
    }
    else if (number > 0)
    {
        LOG_DEBUG << number << " active event";
        FillActiveChannelList(number, channel_list);
    }
    else
    {
        return true;
    }
    return false;
}

void MultiEpoll::FillActiveChannelList(int number, ActiveChannelList* channel_list)
{
    for (int i = 0; i < number; ++i)
    {
        auto channel = static_cast<Channel*>(events_[i].data.ptr);
        if (channel == nullptr)
        {
            LOG_ERROR << "static_cast event.data.ptr error";
        }
        channel->SetEvents(events_[i].events);
        channel_list->push_back(channel);
    }
}
void MultiEpoll::Update(int ctl, Channel* channel)
{
    const int fd = channel->GetFd();

    if (ctl == EPOLL_CTL_DEL)
    {
        epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, nullptr);
    }
    else
    {
        // EPOLL_CTL_ADD EPOLL_CTL_MOD

        epoll_event ev{};
        ev.data.ptr = static_cast<void*>(channel);
        ev.events = channel->GetREvents();
        epoll_ctl(epollfd_, ctl, fd, &ev);
    }
}
