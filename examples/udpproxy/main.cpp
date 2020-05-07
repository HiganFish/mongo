//
// Created by lsmg on 5/5/20.
//

#include <Logger.h>
#include <UdpServer.h>
#include <TcpClient.h>
#include <TcpConnection.h>
#include <EventLoop.h>
#include "Codec.h"

mongo::net::TcpClient* client_server;
Codec codec;

void OnUdpMessage(const mongo::net::UdpDgramPtr& dgram)
{
	Response* response = new Response();
	codec.Decode(response, dgram->GetInputBuffer());

	client_server->NewConnection(response->addr, response->port, response);
}

void OnConnection(const mongo::net::TcpConnectionPtr& conn)
{
	Response* response = static_cast<Response*>(conn->GetArg());
	conn->Send(response->data, response->data_len);
}

void OnTcpMessage(const mongo::net::TcpConnectionPtr& conn, mongo::net::Buffer* buffer)
{
	// Response* response = static_cast<Response*>(conn->GetArg());

	LOG_INFO << buffer->ReadableBytes() << "\r\n" << buffer->ReadAllAsString();
}

int main()
{
	mongo::net::EventLoop loop;
	mongo::net::TcpClient client(&loop, "proxy-client-server");
	client.SetNewConnectionCallback(OnConnection);
	client.SetMessageCallback(OnTcpMessage);
	client_server = &client;

	mongo::net::InetAddress addr{19991};
	mongo::net::UdpServer server("udpserver-test", addr);
	server.SetOnMessageCallback(OnUdpMessage);

	loop.RunInLoop(std::bind(&mongo::net::UdpServer::LoopOnce, &server), true);

	client.Start();
	loop.Loop();
}