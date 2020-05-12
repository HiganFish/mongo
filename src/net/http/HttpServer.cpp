//
// Created by lsmg on 5/11/20.
//

#include "HttpServer.h"

using namespace mongo;
using namespace mongo::net;

HttpServer::HttpServer(EventLoop* loop, const std::string& name, const InetAddress& addr):
server_(loop, name, addr)
{
	server_.SetNewConnectionCallback(std::bind(&HttpServer::OnConnection, this, _1));
	server_.SetMessageCallback(std::bind(&HttpServer::OnMessageCallback, this, _1, _2, _3));
}
HttpServer::~HttpServer()
{

}
void HttpServer::OnMessageCallback(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp recv_time)
{

}
void HttpServer::OnConnection(const TcpConnectionPtr& conn)
{

}
