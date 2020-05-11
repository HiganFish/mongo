//
// Created by lsmg on 5/5/20.
//

#include "UdpServer.h"
#include "Logger.h"

using namespace mongo;
using namespace mongo::net;

UdpServer::UdpServer(const std::string& name, const InetAddress& addr):
	server_name_(name),
	peer_addr_(addr),
	bindfd_(sockets::CreateUdpFd()),
	onmessage_callback(std::bind(&UdpServer::DefaultOnMessage, this, std::placeholders::_1)),
	looping_(false)
{
	bindfd_.SetRecvTimeout(1);
	bindfd_.Bind(addr);
	LOG_INFO << "Bind on " << addr.GetIpPort();
}

void UdpServer::LoopOnce()
{
	UdpDgramPtr dgram(new UdpDgram(this, peer_addr_));

	bool result = dgram->RecvFrom(bindfd_);

	if (result)
	{
		if (onmessage_callback)
		{
			onmessage_callback(dgram);
		}

	}
}
void UdpServer::DefaultOnMessage(const UdpDgramPtr& dgram)
{
	LOG_INFO << dgram->GetInputBuffer()->ReadableBytes() << " \r\n" <<
		  dgram->GetInputBuffer()->ReadAllAsString();
}
void UdpServer::Loop()
{
	while (looping_)
	{
		LoopOnce();
	}
}
