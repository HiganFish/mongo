//
// Created by lsmg on 5/12/20.
//

#include <mongo/net/http/HttpServer.h>
#include <mongo/net/http/HttpRequest.h>
#include <mongo/net/http/HttpResponse.h>
#include <mongo/net/EventLoop.h>
#include <mongo/base/Logger.h>
#include <mongo/base/Daemon.h>

std::string base_url;

bool sample_response = false;

void OnMessage(const mongo::net::HttpRequest& request, const mongo::net::HttpServer::HttpResponsePtr& response)
{
	std::string url = base_url + request.GetPath();
	response->SetResponseCode(mongo::net::HttpResponse::OK200);
	response->SetResponseMessage("OK");
	(*response)["Server"] = "哔哩哔哩 (゜-゜)つロ 干杯~-bilibili";

	if (sample_response)
	{
		response->SetResponseBody(R"({"status": 1})");
	}
	else
	{
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
}

int main(int argc, char* argv[])
{
	LOG_FATAL_IF(argc < 3) << "./xxx web_root port";

	base_url += argv[1];
	short port = atoi(argv[2]);

	LOG_FATAL_IF(port < 1024) << "invalid port " << port;

	// mongo::Logger::SetLogLevel(mongo::Logger::LogLevel::DEBUG);
	mongo::Logger::SetLogPlace(mongo::Logger::FILE, "HttpServerTest", "/home/lsmg");
	mongo::Daemon::InitDaemon();

	mongo::net::EventLoop loop;

	mongo::net::HttpServer server(&loop, "http-test", mongo::net::InetAddress(port));
	server.SetHttpMessageCallback(OnMessage);
	// server.SetExThreadNum(3);
	server.Start();
	loop.Loop();
};