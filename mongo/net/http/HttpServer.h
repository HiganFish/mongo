//
// Created by lsmg on 5/11/20.
//

#ifndef _HTTPSERVER_H_
#define _HTTPSERVER_H_

#include "mongo/net/TcpServer.h"
#include "mongo/base/MutexGuard.h"

namespace mongo
{
namespace net
{

class HttpContext;
class HttpResponse;
class HttpRequest;
class HttpServer : noncopyable
{
public:
	typedef std::shared_ptr<HttpContext> HttpContextPtr;
	typedef std::function<void(const HttpRequest& request, HttpResponse* response)> HttpMessageCallback;

	HttpServer(EventLoop* loop, const std::string& name, const InetAddress& addr);
	~HttpServer();

	void SetHttpMessageCallback(const HttpMessageCallback& callback)
	{ httpmessage_callback_ = callback; }

	void Start();
private:

	TcpServer server_;


	typedef std::map<std::string, HttpContextPtr> HttpContextMap;

	Mutex context_map_mutex_;
	HttpContextMap context_map_;

	HttpMessageCallback httpmessage_callback_;

	void OnMessageCallback(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp recv_time);
	void OnHttpMessage(const TcpConnectionPtr& conn, const HttpRequest& request);

	void OnConnection(const TcpConnectionPtr& conn);

	void OnCloseConnection(const TcpConnectionPtr& conn);

	void OnWriteBody(const TcpConnectionPtr& conn);

	void CloseConnection(const TcpConnectionPtr& conn);
};
}
}

#endif //_HTTPSERVER_H_
