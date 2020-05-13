//
// Created by lsmg on 5/12/20.
//

#include <mongo/net/http/HttpServer.h>
#include <mongo/net/http/HttpRequest.h>
#include <mongo/net/http/HttpResponse.h>
#include <mongo/net/EventLoop.h>

void OnMessage(const mongo::net::HttpRequest& request, mongo::net::HttpResponse* response)
{
	response->SetResponseCode(mongo::net::HttpResponse::OK200);
	response->SetResponseMessage("OK");
	response->SetContextType(mongo::net::HttpResponse::TEXT_HTML);
	response->SetResponseBody("<html><head><title>Example</title></head><body>"
							  "<p>Hello World</p>"
							  "</body></html>");
}

int main()
{
	mongo::net::EventLoop loop;
	mongo::net::HttpServer server(&loop, "http-test", mongo::net::InetAddress(8112));
	server.SetHttpMessageCallback(OnMessage);
	server.Start();
	loop.Loop();
};