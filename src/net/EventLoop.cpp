//
// Created by lsmg on 4/5/20.
//

#include "EventLoop.h"
#include "multiplexing/MultiBase.h"
#include "Channel.h"

using namespace mongo;
using namespace mongo::net;

EventLoop::EventLoop():
multi_base_(MultiBase::InitMulti(this)),
looping_(false),
timer()
{

}
EventLoop::~EventLoop()
{

}
void EventLoop::Loop()
{
    looping_ = true;
    timer.StartTimerLoop();

    while (looping_)
    {
        active_channel_list.clear();

        bool has_timeout = multi_base_->LoopOnceWithTimeout(timer.GetNextTime(), &active_channel_list);

        if (has_timeout)
        {
            timer.ProcessReadyTask();
        }

        for (Channel* channel : active_channel_list)
        {
            channel->HandleEvent();
        }
    }
}
void EventLoop::UpdateChannel(Channel* channel)
{
    if (channel->GetTimerAddOnce())
    {
        timer.AddTimer(*channel->GetTimerTask());
    }

    multi_base_->UpdateChannel(channel);
}