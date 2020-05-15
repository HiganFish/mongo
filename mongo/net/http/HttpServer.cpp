//
// Created by lsmg on 5/11/20.
//

#include "HttpServer.h"
#include "HttpContext.h"
#include "HttpResponse.h"

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
	HttpContextMap::const_iterator iter = context_map_.find(conn->GetConnectionName());
	if (iter == context_map_.end())
	{
		LOG_ERROR << "Unknow context key " << conn->GetConnectionName();
		return;
	}
	HttpContextPtr context = iter->second;

	if (!context->Parse(buffer, recv_time))
	{
		return;
	}

	if (context->ParseOver())
	{
		if (httpmessage_callback_)
		{
			OnHttpMessage(conn, context->GetRequest());
			context->Reset();
		}
	}
}
void HttpServer::OnConnection(const TcpConnectionPtr& conn)
{
	{
		MutexGuard guard(context_map_mutex_);
		context_map_[conn->GetConnectionName()].reset(new HttpContext());
	}
}
void HttpServer::OnHttpMessage(const TcpConnectionPtr& conn, const HttpRequest& request)
{
	const std::string& connection = request.GetHeader("Connection");
	bool close = (connection == "close") ||
				 (request.GetVersion() == HttpRequest::HTTP10);

	HttpResponse* response = new HttpResponse(close);
	if (httpmessage_callback_)
	{
		httpmessage_callback_(request, response);
	}

	/**
	 * 发送Html头部 未用户设置的body文件
	 */
	Buffer header_buffer;
	response->EncodeToBuffer(&header_buffer);
	conn->Send(&header_buffer);

	/**
	 * 存在用户设置的body文件 则注册可写回调用于发送body文件
	 */
	if (response->HasFileBody())
	{
		conn->SetWritableCallback(std::bind(&HttpServer::OnWriteBody, this, _1));
		conn->EnableWriting();
		conn->SetArg(response);
	}
	else
	{
		if (close)
		{
			CloseConnection(conn);
		}
	}
}
void HttpServer::OnCloseConnection(const TcpConnectionPtr& conn)
{
	MutexGuard guard(context_map_mutex_);
	context_map_.erase(conn->GetConnectionName());
}
void HttpServer::Start()
{
	server_.Start();
}

void HttpServer::OnWriteBody(const TcpConnectionPtr& conn)
{
	HttpResponse* response = static_cast<HttpResponse*>(conn->GetArg());
	if (!response)
	{
		return;
	}

	Buffer body_buffer;
	bool read_over = response->ReadBodyToBuffer(&body_buffer);

	conn->Send(&body_buffer);

	if (read_over)
	{
		delete response;
		conn->SetArg(nullptr);

		conn->DisableWriting();
		if (response->IsCloseConnection())
		{
			CloseConnection(conn);
		}
	}
}
void HttpServer::CloseConnection(const TcpConnectionPtr& conn)
{
	conn->CloseConnection();
	MutexGuard guard(context_map_mutex_);
	context_map_.erase(conn->GetConnectionName());
}
