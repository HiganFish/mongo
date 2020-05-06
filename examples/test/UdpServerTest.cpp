//
// Created by lsmg on 5/5/20.
//

#include <Logger.h>
#include <UdpServer.h>

std::string response = "HTTP/1.1 200 OK\r\n"
					   "Content-Length: 48\r\n"
					   "Content-Type: text/html\r\n"
					   "Connection: Closed\r\n\r\n"
					   "<html>"
					   "<body>"
					   "<h1>Hello, World!</h1>"
					   "</body>"
					   "</html>";

void OnMessage(const mongo::net::UdpDgramPtr& dgram)
{
	LOG_INFO << dgram->GetInputBuffer()->ReadableBytes() << " \r\n" <<
			 dgram->GetInputBuffer()->ReadAllAsString();

	dgram->Send(response);
}


int main()
{

	mongo::net::InetAddress addr{19991};

	mongo::net::UdpServer server("udpserver-test", addr);
	server.SetOnMessageCallback(OnMessage);
	server.Start();
}