//
// Created by lsmg on 5/3/20.
//
#include <mongo/net/TcpServer.h>
#include <mongo/net/EventLoop.h>
#include <mongo/net/TcpConnection.h>

void OnConnection(const mongo::net::TcpConnectionPtr& conn)
{
	conn->EnableAutoClose(5);
}

int main()
{
	mongo::net::EventLoop loop;
	mongo::net::InetAddress addr(8000);
	mongo::net::TcpServer server(&loop, "TcpServerTest", addr);

	server.SetNewConnectionCallback(OnConnection);

	server.Start();
	loop.Loop();
}