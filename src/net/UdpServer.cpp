//
// Created by lsmg on 5/5/20.
//

#include "UdpDgram.h"
#include "UdpServer.h"
#include "Logger.h"

using namespace mongo;
using namespace mongo::net;

UdpServer::UdpServer(const std::string& name, const InetAddress& addr):
server_name_(name),
peer_addr_(addr),
listenfd_(sockets::CreateUdpFd()),
looping_(false)
{
	listenfd_.Bind(addr);
	LOG_INFO << "Bind on " << addr.GetIpPort();
}
void UdpServer::Start()
{
	Loop();
}
void UdpServer::Loop()
{
	looping_ = true;

	while (looping_)
	{
		UdpDgram dgram(peer_addr_);

		bool result = dgram.RecvFrom(listenfd_);

		if (result)
		{
			LOG_INFO << dgram.GetInputBuffer()->ReadableBytes() << " \r\n" <<
			dgram.GetInputBuffer()->ReadAllAsString();
		}
	}
}
