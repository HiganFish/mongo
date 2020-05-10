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
	response->dgram = dgram;

	codec.Decode(response, dgram->GetInputBuffer());

	LOG_INFO << "Recv-"<< response->uuid;

	client_server->GetConnection(response->addr, response->port, response);
}

void OnConnection(const mongo::net::TcpConnectionPtr& conn)
{
	Response* response = static_cast<Response*>(conn->GetArg());
	conn->Send(response->data, response->data_len);

	mongo::net::Buffer* InputBuffer = conn->GetInputBuffer();
	InputBuffer->Append(response->uuid, 37);
	LOG_INFO << "Conn-"<< response->uuid;
}

void OnClose(const mongo::net::TcpConnectionPtr& conn)
{
	Response* response = static_cast<Response*>(conn->GetArg());
	LOG_INFO << "Delete-" << response->uuid;

	delete response;
}

void OnTcpMessage(const mongo::net::TcpConnectionPtr& conn, mongo::net::Buffer* buffer)
{
	Response* response = static_cast<Response*>(conn->GetArg());
	buffer->RAppendInt32(buffer->ReadableBytes());

	LOG_INFO << "Reply-" << response->uuid << " " << buffer->ReadableBytes() - 41;
	response->dgram->Send(buffer->ReadBegin(), buffer->ReadableBytes());
	buffer->DropAllData();
	buffer->Append(response->uuid, 37);
}

int main()
{
	mongo::net::EventLoop loop;
	mongo::net::TcpClient client(&loop, "proxy-client-server");
	client.SetNewConnectionCallback(OnConnection);
	client.SetMessageCallback(OnTcpMessage);
	client.SetCloseCallback(OnClose);
	client_server = &client;

	mongo::net::InetAddress addr{19991};
	mongo::net::UdpServer server("udpserver-test", addr);
	server.SetOnMessageCallback(OnUdpMessage);

	loop.RunInLoop(std::bind(&mongo::net::UdpServer::LoopOnce, &server), true);

	client.Start();
	loop.Loop();
}