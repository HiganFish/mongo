//
// Created by lsmg on 4/30/20.
//

#include <TcpServer.h>
#include <EventLoop.h>
#include <TcpConnection.h>
#include <Logger.h>
#include <Timestamp.h>

void OnTimeOver(const mongo::net::TcpConnectionPtr& conn, void* arg)
{
    LOG_INFO << "connection " <<  conn->GetConnectionName() << " time over " <<  mongo::Timestamp::Now().ToSecMsecUsec();
}

void OnNewConnection(const mongo::net::TcpConnectionPtr& conn)
{
    LOG_INFO << "connection " <<  conn->GetConnectionName() << " create " << mongo::Timestamp::Now().ToSecMsecUsec();
    conn->AddTimer(OnTimeOver, 0, 100, true, 10);
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