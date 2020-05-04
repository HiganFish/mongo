//
// Created by lsmg on 5/3/20.
//
#include <TcpServer.h>
#include <EventLoop.h>
#include <TcpConnection.h>

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