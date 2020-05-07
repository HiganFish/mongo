//
// Created by lsmg on 5/7/20.
//

#include "TcpClient.h"

//
// Created by lsmg on 4/6/20.
//
#include <Logger.h>
#include "TcpConnection.h"
#include "EventLoopThreadPoll.h"

using namespace mongo;
using namespace mongo::net;

TcpClient::TcpClient(EventLoop* loop, const std::string& name):
loop_(loop),
poll_(new EventLoopThreadPoll(loop_)),
client_name_(name)
{
	message_callback_ = std::bind(&TcpClient::DefaultMessageCallback, this, std::placeholders::_1, std::placeholders::_2);
	LOG_INFO << "start tcp clients-" << name;
}

void TcpClient::Start()
{
	poll_->Start();
}
void TcpClient::NewConnection(const char* addr, short port, void* arg)
{
	InetAddress server_addr;
	int sockfd = sockets::Connect(&server_addr, addr, port);


	if (sockfd == -1)
	{
		return;
	}

	std::string connection_name = client_name_ + "-" + server_addr.GetIpPort();

	/**
	 * 这一行代码很妙, 单线程时候返回最初传入的事件循环 多线程返回对应的事件循环
	 */
	EventLoop *io_loop = poll_->GetNextEventLoop();

	TcpConnectionPtr connection(new TcpConnection(io_loop, connection_name, sockfd, server_addr, local_hosts_));
	connections[connection_name] = connection;
	connection->SetArg(arg);
	connection->SetMessageCallback(message_callback_);
	connection->SetWriteOverCallback(writeover_callback_);
	connection->SetCloseCallback(std::bind(&TcpClient::CloseConnection, this, std::placeholders::_1));

	LOG_INFO << "connection " << connection_name << " connected";

	connection->ConnectionCreated();
	if (newconnection_callback_)
	{
		newconnection_callback_(connection);
	}
}
void TcpClient::CloseConnection(const TcpConnectionPtr& conn)
{
	connections.erase(conn->GetConnectionName());
	LOG_INFO << "connection " << conn->GetConnectionName() << " closed";

	if (close_callback_)
	{
		close_callback_(conn);
	}
}

void TcpClient::DefaultMessageCallback(const TcpConnectionPtr& conn, Buffer* buffer)
{
	std::string result = buffer->ReadAllAsString();

	LOG_INFO << client_name_.c_str() << " a new msg form " << conn->GetClientAddr().GetIpPort() << '\n' << result.c_str();
}
void TcpClient::SetThreadNum(int nums)
{
	poll_->SetThreadNums(nums);
}
