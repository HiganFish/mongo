//
// Created by lsmg on 5/12/20.
//

#include <mongo/net/http/HttpServer.h>
#include <mongo/net/http/HttpRequest.h>
#include <mongo/net/http/HttpResponse.h>
#include <mongo/net/EventLoop.h>

std::string base_url = "/home/lsmg/web/";

void OnMessage(const mongo::net::HttpRequest& request, const mongo::net::HttpServer::HttpResponsePtr& response)
{
	std::string url = base_url + request.GetPath();
	if (!response->SetBodyFilePath(url))
	{
		response->SetResponseCode(mongo::net::HttpResponse::NOTFOUNT_404);
		response->SetResponseMessage("NOT FOUND");
	}
	else
	{
		response->SetResponseCode(mongo::net::HttpResponse::OK200);
		response->SetResponseMessage("OK");
	}
}

int main()
{
	mongo::net::EventLoop loop;
	mongo::net::HttpServer server(&loop, "http-test", mongo::net::InetAddress(8112));
	server.SetHttpMessageCallback(OnMessage);
	server.SetExThreadNum(4);
	server.Start();
	loop.Loop();
};