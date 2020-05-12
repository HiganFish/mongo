//
// Created by lsmg on 5/11/20.
//

#ifndef _HTTPSERVER_H_
#define _HTTPSERVER_H_

#include "TcpServer.h"

namespace mongo
{
namespace net
{
class HttpServer
{
public:
	HttpServer(EventLoop* loop, const std::string& name, const InetAddress& addr);
	~HttpServer();

private:

	TcpServer server_;

	void OnMessageCallback(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp recv_time);

	void OnConnection(const TcpConnectionPtr& conn);
};
}
}

#endif //_HTTPSERVER_H_
