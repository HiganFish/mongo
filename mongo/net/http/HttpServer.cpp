//
// Created by lsmg on 5/11/20.
//

#include "mongo/net/http/HttpServer.h"
#include "mongo/net/http/HttpContext.h"
#include "mongo/net/http/HttpResponse.h"

#include "mongo/base/Logger.h"
#include "mongo/net/TcpConnection.h"

using namespace mongo;
using namespace mongo::net;

HttpServer::HttpServer(EventLoop* loop, const std::string& name, const InetAddress& addr):
server_(loop, name, addr)
{
	server_.SetNewConnectionCallback(std::bind(&HttpServer::OnConnection, this, _1));
	server_.SetCloseCallback(std::bind(&HttpServer::OnCloseConnection, this, _1));
	server_.SetMessageCallback(std::bind(&HttpServer::OnMessageCallback, this, _1, _2, _3));
}

HttpServer::~HttpServer()
{

}

void HttpServer::OnMessageCallback(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp recv_time)
{
	HttpContextMap::const_iterator iter;
	HttpContextPtr context;
	{
		MutexGuard guard(mutex_context_map_);
		iter = context_map_.find(conn->GetConnectionName());
		if (iter == context_map_.end())
		{
			LOG_ERROR << "Unknow context key " << conn->GetConnectionName();
			return;
		}
		context = iter->second;
	}

	if (!context->Parse(buffer, recv_time))
	{
		return;
	}

	if (context->ParseOver())
	{
		OnHttpMessage(conn, context->GetRequest());
		context->Reset();
	}
}
void HttpServer::OnConnection(const TcpConnectionPtr& conn)
{
	{
		MutexGuard guard(mutex_context_map_);
		context_map_[conn->GetConnectionName()].reset(new HttpContext());
	}
}
void HttpServer::OnHttpMessage(const TcpConnectionPtr& conn, const HttpRequest& request)
{
	const std::string& connection = request.GetHeader("Connection");
	bool close = (connection == "close") ||
				 (request.GetVersion() == HttpRequest::HTTP10);

	HttpResponsePtr response(new HttpResponse(close));

	routing_[request](request, response);

	/**
	 * 发送HTTP头部 未用户设置的body文件
	 */
	Buffer header_buffer;
	response->EncodeToBuffer(&header_buffer);
	conn->Send(&header_buffer);
	/**
	 * 存在用户设置的body文件
	 */
	if (response->HasFileBody())
	{
		/**
		 * body文件未发送完毕 注册回调函数
		 */
		if (!SendBodyAndTryClose(conn, response))
		{
			conn->SetWritableCallback(std::bind(&HttpServer::OnWriteBody, this, _1, response));
			conn->EnableWriting();
		}
	}
	else
	{
		TryCloseConnection(conn, response);
	}
}
void HttpServer::OnCloseConnection(const TcpConnectionPtr& conn)
{
	MutexGuard guard(mutex_context_map_);
	context_map_.erase(conn->GetConnectionName());
}
void HttpServer::Start()
{
	server_.Start();
}

void HttpServer::OnWriteBody(const TcpConnectionPtr& conn, const HttpResponsePtr& response)
{
	if (!response)
	{
		return;
	}

	bool read_over = SendBodyAndTryClose(conn, response);

	if (read_over)
	{
		conn->DisableWriting();
	}
}
void HttpServer::TryCloseConnection(const TcpConnectionPtr& conn, const HttpResponsePtr& response)
{
	if (response->IsCloseConnection())
	{
		conn->CloseConnection();
		MutexGuard guard(mutex_context_map_);
		context_map_.erase(conn->GetConnectionName());
	}
}

void HttpServer::SetExThreadNum(int nums)
{
	server_.SetThreadNum(nums);
}

bool HttpServer::SendBodyAndTryClose(const TcpConnectionPtr& conn, const HttpResponsePtr& response)
{
	Buffer body_buffer;
	bool read_over = response->ReadBodyToBuffer(&body_buffer);

	conn->Send(&body_buffer);

	if (read_over)
	{
		TryCloseConnection(conn, response);
	}

	return read_over;
}

HttpRouting& HttpServer::operator[](const std::string& url)
{
	LOG_FATAL_IF(url.empty()) << "Routing url can't be empty";

	routing_.SetUrlForAdd(url);

	return routing_;
}
