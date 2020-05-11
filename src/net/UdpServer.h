//
// Created by lsmg on 5/5/20.
//

#ifndef _UDPSERVER_H_
#define _UDPSERVER_H_

#include <functional>
#include <string>
#include <memory>
#include "Socket.h"
#include "UdpDgram.h"

namespace mongo
{
namespace net
{

typedef std::shared_ptr<UdpDgram> UdpDgramPtr;

class UdpServer : noncopyable
{
public:
	typedef std::function<void(const UdpDgramPtr& dgram)> OnMessage;

	UdpServer(const std::string& name, const InetAddress& addr);

	void SetOnMessageCallback(const OnMessage& cb)
	{ onmessage_callback = cb; }

	void LoopOnce();

	void Loop();

	Socket* GetBindFd()
	{ return &bindfd_; }

private:
	std::string server_name_;
	InetAddress peer_addr_;

	Socket bindfd_;

	OnMessage onmessage_callback;

	bool looping_;

	void DefaultOnMessage(const UdpDgramPtr& dgram);

};
}
}

#endif //_UDPSERVER_H_
