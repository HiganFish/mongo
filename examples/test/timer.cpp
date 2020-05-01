//
// Created by lsmg on 4/30/20.
//

#include <TcpServer.h>
#include <EventLoop.h>
#include <TcpConnection.h>
#include <Logger.h>
#include <Timestamp.h>

void OnTimeOver1(const mongo::net::TcpConnectionPtr& conn, void* arg)
{
    LOG_INFO << "OnTimeOver1 tigger";
}

void OnTimeOver2(const mongo::net::TcpConnectionPtr& conn, void* arg)
{
    LOG_INFO << "OnTimeOver2 tigger";
}
void OnNewConnection(const mongo::net::TcpConnectionPtr& conn)
{
    conn->AddTimer(OnTimeOver1, nullptr,  "1", 1, 0, false);
    conn->AddTimer(OnTimeOver2, nullptr,  "2", 2, 0, false);
}

int main()
{

    mongo::net::InetAddress address{9000};
    mongo::net::EventLoop loop;

    mongo::net::TcpServer server(&loop, "timer_test", address);
    server.SetNewConnectionCallback(OnNewConnection);

    server.Start();
    loop.Loop();
}