//
// Created by lsmg on 5/5/20.
//

#ifndef _UDPSERVER_H_
#define _UDPSERVER_H_

#include <string>
#include "Socket.h"

namespace mongo
{
namespace net
{
class UdpServer : noncopyable
{
public:
	UdpServer(const std::string& name, const InetAddress& addr);

	void Start();
private:
	std::string server_name_;
	InetAddress peer_addr_;

	Socket listenfd_;

	bool looping_;
	void Loop();
};
}
}

#endif //_UDPSERVER_H_
