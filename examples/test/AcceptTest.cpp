//
// Created by lsmg on 4/6/20.
//
#include <mongo/net/EventLoop.h>
#include <mongo/net/TcpServer.h>

int main()
{
    mongo::net::EventLoop loop;
    mongo::net::InetAddress addr(8000);
    mongo::net::TcpServer server(&loop, "test", addr);
    server.SetThreadNum(4);
    server.Start();
    loop.Loop();
}