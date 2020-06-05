//
// Created by lsmg on 5/11/20.
//

#ifndef _HTTPSERVER_H_
#define _HTTPSERVER_H_

#include "mongo/net/TcpServer.h"
#include "mongo/net/http/HttpRouting.h"

namespace mongo
{
namespace net
{
class HttpServer : noncopyable
{
public:
	HttpServer(EventLoop* loop, const std::string& name, const InetAddress& addr);
	~HttpServer();

	void SetDefaultRequestCallback(const HttpRequestCallback& callback)
	{
		routing_.SetDefaultMessageCallback(callback); }

	void SetExThreadNum(int nums);

	void Start();

	HttpRouting& operator[](const std::string& url);

private:

	TcpServer server_;

	HttpRouting routing_;

	typedef std::map<std::string, HttpContextPtr> HttpContextMap;

	Mutex mutex_context_map_;
	HttpContextMap context_map_;

	void OnMessageCallback(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp recv_time);
	void OnHttpMessage(const TcpConnectionPtr& conn, const HttpRequest& request);

	void OnConnection(const TcpConnectionPtr& conn);

	void OnCloseConnection(const TcpConnectionPtr& conn);

	void OnWriteBody(const TcpConnectionPtr& conn, const HttpResponsePtr& response);

	void TryCloseConnection(const TcpConnectionPtr& conn, const HttpResponsePtr& response);

	bool SendBodyAndTryClose(const TcpConnectionPtr& conn, const HttpResponsePtr& response);

};
}
}

#endif //_HTTPSERVER_H_
