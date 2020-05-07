//
// Created by lsmg on 4/29/20.
//

#include <sys/time.h>
#include "Timer.h"
#include "Channel.h"

using namespace mongo;
using namespace mongo::net;

Timer::Timer():
running_(false),
default_return_time_(20),
running_time_(0),
running_ex_usec_(0)
{

}

Timer::~Timer()
{

}

bool Timer::AddTimer(const TimerTask& task)
{
    AddTimer(task.channel, task.key, 0, task.set_msec, task.repeat, task.count);
}


bool Timer::AddTimer(Channel* channel, const std::string& key_, int sec, bool repeat, int count)
{
    return AddTimer(channel, key_, sec, 0, repeat, count);
}

bool Timer::AddTimer(Channel* channel, const std::string& key_, int sec, int msec, bool repeat, int count)
{
    if ((sec < 0) || (msec < 0))
    {
        return false;
    }

    if (repeat && count == 0)
    {
        return false;
    }

    if (running_)
    {
        struct timeval now;
        gettimeofday(&now, nullptr);
        long long extime = (now.tv_sec - start_time_.tv_sec) * 1000 + (now.tv_usec - start_time_.tv_usec) / 1000;
        task_queue_.emplace(channel, key_, sec * 1000 + msec, sec * 1000 + msec + extime, repeat, count);
    }
    else
    {
        task_queue_.emplace(channel, key_, sec * 1000 + msec, sec * 1000 + msec, repeat, count);
    }
    return true;
}



void Timer::StartTimerLoop()
{
    running_ = true;
    gettimeofday(&start_time_, nullptr);
}

int Timer::GetNextTime()
{
    if (task_queue_.empty())
    {
        return default_return_time_;
    }

    const TimerTask& task = task_queue_.top();

    gettimeofday(&now_time_, nullptr);

    /**
     * 保存微妙部分 足够1ms后 将运行时间增加1ms
     */
    running_time_ = (now_time_.tv_sec - start_time_.tv_sec) * 1000 + (now_time_.tv_usec - start_time_.tv_usec) / 1000;
    running_ex_usec_ +=  static_cast<int>(now_time_.tv_usec - start_time_.tv_usec) % 1000;
    if (running_ex_usec_ >= 1000)
    {
        running_time_ += running_ex_usec_ % 1000;
        running_ex_usec_ /= 1000;
    }

    int timeout = static_cast<int>(task.next_msec - running_time_);

    return timeout;
}

void Timer::ResetTimer(const TimerTask& task)
{
    int remain_count = task.count - 1;
    if (remain_count > 0)
    {
        task_queue_.emplace(task.channel, task.key, task.set_msec, task.next_msec + task.set_msec, task.repeat, task.count - 1);
    }
    if (remain_count == 0)
    {
        return;
    }
    if (remain_count == -2)
    {
        task_queue_.emplace(task.channel, task.key, task.set_msec, task.next_msec + task.set_msec, task.repeat, -1);
    }
}

void Timer::ProcessReadyTask()
{
    if (task_queue_.empty())
    {
        return;
    }

    /**
     * 用于将所有就绪事件保存到 事件就绪队列 减少循环次数
     */
    long long sum_time = task_queue_.top().next_msec;
    while ((!task_queue_.empty()) && (task_queue_.top().next_msec <= sum_time))
    {
        task_ready_deque_.push_back(task_queue_.top());
        task_queue_.pop();
    }


    while (!task_ready_deque_.empty())
    {
        const TimerTask& task = task_ready_deque_.front();
        task_ready_deque_.pop_front();
        if (task.repeat)
        {
            ResetTimer(task);
        }
        if (task.channel)
        {
            task.channel->TimerOver(task.key);
        }
    }
}