//
// Created by lsmg on 4/5/20.
//

#ifndef MONGO_SRC_NET_EVENTLOOP_H
#define MONGO_SRC_NET_EVENTLOOP_H

#include <vector>
#include <memory>

#include "Timer.h"

namespace mongo
{
namespace net
{
class Channel;
class MultiBase;
class EventLoop
{
public:
	typedef std::function<void()> LoopFunc;

    EventLoop();
    ~EventLoop();

    /**
     * under normal conditions endless loop
     */
    void Loop();

    void UpdateChannel(Channel *channel);

    void RunInLoop(const LoopFunc& func, bool repeat = false);

private:
    bool looping_;
    std::unique_ptr<MultiBase> multi_base_;
    typedef std::vector<Channel*> ActiveChannelList;
    ActiveChannelList active_channel_list;

    std::vector<LoopFunc> loop_func_queue_;
    std::queue<LoopFunc> loop_once_func_queue_;

    Timer timer;

    void CallLoopFunc();
};
}
}

#endif //MONGO_SRC_NET_EVENTLOOP_H
