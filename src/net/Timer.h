//
// Created by lsmg on 4/29/20.
//

#ifndef UNTITLED__TIMER_H
#define UNTITLED__TIMER_H

#include <sys/epoll.h>
#include <queue>
#include <functional>

namespace mongo{
namespace net
{
class Channel;

struct TimerTask
{
    TimerTask(
        Channel* channel_,
        int set_msec_,
        long long next_msec_,
        bool reset_,
        int count_) :
        channel(channel_),
        set_msec(set_msec_),
        next_msec(next_msec_),
        repeat(reset_),
        count(count_)
    {
    }
    Channel* channel;
    int set_msec; /* 用户设定的定时时间 单位毫秒 */
    long long next_msec; /* 下次触发时间 单位毫秒 */
    bool repeat; /* 是否循环 */
    int count; /* 循环剩余 -1表示无限循环 */

    /**
     * 用于设定最小堆
     */
    bool operator<(const TimerTask& task) const
    {
        return set_msec > task.set_msec;
    }
};
class Timer
{
public:

    typedef std::function<void(void* args)> TimeOverCallback;

    Timer();
    ~Timer();

    /**
     * 增加定时器 默认msec为0
     * @param sec 秒
     * @param callback 回调函数
     * @param args 回调函数参数
     * @param repeat 是否重复 可选 默认不重复
     * @param count 总执行次数 可选 默认-1无限次数
     * @return
     */

    bool AddTimer(const TimerTask& task);

    bool AddTimer(Channel* channel, int sec, bool repeat = false, int count = -1);

    /**
     * 增加定时器
     * @param sec 秒
     * @param msec 毫秒
     * @param callback 回调函数
     * @param args 回调函数参数
     * @param repeat 是否重复 可选 默认不重复
     * @param count 总执行次数 可选 默认-1无限次数
     * @return
     */
    bool AddTimer(Channel* channel, int sec, int msec, bool repeat = false, int count = -1);

    /**
     * 开始时间循环
     */
    void StartTimerLoop();

    /**
     * 如果存在事件则返回最小超时时间, 否则返回默认值10ms
     * @return
     */
    int GetNextTime();

    void ProcessReadyTask();
private:

    int default_return_time_;

    /**
     * 当前是否在运行， 运行后设定的定时时间需要增加 running_time_
     */
    bool running_;

    /**
     * 定时器已经运行时间 单位毫秒
     */
    long long running_time_;

    /*
     * 由于epoll_wait 毫秒级定时器
     * 用于保存舍去的微妙部分， 当足够进位的时候 进行进位操作
     */
    int running_ex_usec_;

    /**
     * 保存循环开始时间 用于作为基值计算运行时间时间差
     */
    struct timeval start_time_;
    struct timeval now_time_;

    /**
     * 事件循环队列
     */
    std::priority_queue<TimerTask> task_queue_;

    /**
     * 事件就绪队列
     */
    std::deque<TimerTask> task_ready_deque_;

    void ResetTimer(const TimerTask& task);

private:
    /**
     * Muti
     */
};
}
}
#endif //UNTITLED__TIMER_H
