//
// Created by lsmg on 4/5/20.
//

#ifndef MONGO_SRC_NET_CHANNEL_H
#define MONGO_SRC_NET_CHANNEL_H

#include <cstdint>
#include <functional>
#include <memory>
#include <sys/epoll.h>
#include <map>
namespace mongo
{
namespace net
{

class EventLoop;
struct TimerTask;

class Channel
{
public:
    typedef std::function<void()> EventCallback;
    typedef std::function<void(const std::string& key)> TimeOverCallback;

    enum ChannelStatus {ADD, ADDED, DELED};

    Channel(EventLoop *loop, int fd);
    ~Channel();

    ChannelStatus GetStatus() const
    { return status_; }

    void SetStatus(ChannelStatus status)
    { status_ = status; }

    int GetFd() const
    { return fd_; }

    uint32_t GetREvents() const
    { return revents_; }

    void SetEvents(uint32_t event)
    { events_ = event; }

    void EnableReading()
    { revents_ |= EPOLLIN; Update(); }
    void DisableReading()
    { revents_ &= ~EPOLLIN; Update(); }

    void EnableWriting()
    { revents_ |= EPOLLOUT; Update(); }
    void DisableWriting()
    { revents_ &= ~EPOLLOUT; Update(); }

    void AddTimer(const TimeOverCallback& callback, const std::string& key, int sec, int msec, bool repeat = false, int count = -1);
    // void DisableTimer(int sec, int msec, bool repeat = false, int count = -1);

    void DisableAll()
    { revents_ = 0; Update(); }

    void HandleEvent();

    void SetReadCallback(const EventCallback& read_callback)
    { read_callback_ = read_callback; }

    void SetWriteCallback(const EventCallback& write_callback)
    { write_callback_ = write_callback; }

    void SetErrorCallback(const EventCallback& error_callback)
    { error_callback_ = error_callback; }

    void SetCloseCallback(const EventCallback& close_callback)
    { close_callback_ = close_callback; }

    void TimerOver(const std::string& key);

    std::string GetNewTimerKey()
    {
        std::string temp = new_timer_key_;
        new_timer_key_.clear();
        return temp;
    }

    std::shared_ptr<TimerTask> GetTimerTaskByKey(const std::string& key)
    { return timer_task_map_[key]; }
private:

    EventLoop *loop_;

    const int fd_;

    /**
     * 回调事件
     */
    uint32_t events_;

    /**
     * epoll 事件
     */
    uint32_t revents_;
    ChannelStatus status_;

    EventCallback read_callback_;
    EventCallback write_callback_;
    EventCallback error_callback_;
    EventCallback close_callback_;

    std::string new_timer_key_;
    std::map<std::string, std::shared_ptr<TimerTask>> timer_task_map_;
    std::map<std::string, TimeOverCallback> timer_callback_map_;
    // TimeOverCallback time_over_callback_;

    void Update();
};

}
}

#endif //MONGO_SRC_NET_CHANNEL_H
