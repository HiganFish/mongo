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
		CallLoopFunc();
    }

}
void EventLoop::UpdateChannel(Channel* channel)
{
    std::string key = channel->GetNewTimerKey();
    if (!key.empty())
    {
        timer.AddTimer(*channel->GetTimerTaskByKey(key));
    }

    multi_base_->UpdateChannel(channel);
}
void EventLoop::RunInLoop(const EventLoop::LoopFunc& func, bool repeat)
{
	if (repeat)
	{
		loop_func_queue_.push_back(func);
	}
	else
	{
		loop_once_func_queue_.push(func);
	}
}
void EventLoop::CallLoopFunc()
{
	for (auto& func : loop_func_queue_)
	{
		func();
	}

	while (!loop_once_func_queue_.empty())
	{
		loop_once_func_queue_.front()();

		loop_once_func_queue_.pop();
	}
}
