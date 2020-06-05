//
// Created by lsmg on 4/6/20.
//

#include "mongo/base/Logger.h"
#include "mongo/net/Acceptor.h"

#include <csignal>

using namespace mongo;
using namespace mongo::net;

Acceptor::Acceptor(EventLoop* loop, const InetAddress& addr, bool reuse_port):
loop_(loop),
listenfd_(sockets::CreateNonBlockFd()),
channel_(loop, "Acceptor", listenfd_.GetFd())
{
    listenfd_.SetReuseAddr(true);
    listenfd_.SetKeepAlive(true);
    listenfd_.SetReusePort(reuse_port);
    listenfd_.Bind(addr.GetAddr());
    LOG_INFO << "Bind on " << addr.GetIpPort();

	signal(SIGPIPE, SIG_IGN);
}
Acceptor::~Acceptor()
{

}
void Acceptor::Listen()
{
    listenfd_.Listening();
    channel_.SetReadCallback(std::bind(&Acceptor::HandleRead, this));
    channel_.EnableReading();
}
void Acceptor::HandleRead()
{
    InetAddress addr;
    int fd = listenfd_.Accept(&addr);

    if (newconnection_callback_)
    {
        newconnection_callback_(fd, addr);
    }
}
