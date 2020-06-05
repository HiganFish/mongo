//
// Created by lsmg on 4/5/20.
//

#include "mongo/net/multiplexing/MultiBase.h"
#include "mongo/net/multiplexing/MultiEpoll.h"

using namespace mongo;
using namespace mongo::net;

MultiBase::MultiBase(EventLoop* loop):
loop_(loop)
{

}
MultiBase::~MultiBase()
{

}
void MultiBase::UpdateChannel(Channel* channel)
{

}

bool MultiBase::LoopOnceWithTimeout(int msec, ActiveChannelList* channel_list)
{

}
MultiBase* MultiBase::InitMulti(EventLoop* loop)
{
    return new MultiEpoll(loop);
}
