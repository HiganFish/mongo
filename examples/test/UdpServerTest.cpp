//
// Created by lsmg on 5/5/20.
//

#include <UdpServer.h>

int main()
{

	mongo::net::InetAddress addr{19991};

	mongo::net::UdpServer server("udpserver-test", addr);

	server.Start();
}