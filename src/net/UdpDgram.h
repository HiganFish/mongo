//
// Created by lsmg on 5/5/20.
//

#ifndef _UDPDGRAM_H_
#define _UDPDGRAM_H_

#include "Buffer.h"
#include "InetAddress.h"
#include "Socket.h"

namespace mongo
{
namespace net
{

class UdpServer;
class UdpDgram
{
public:
	UdpDgram(UdpServer* server, const InetAddress& peer_addr);

	void Send(const char* str, size_t len);

	void Send(const std::string& str);

	bool RecvFrom(const Socket& sockfd);

	Buffer* GetInputBuffer()
	{ return &input_buffer_; }

	Buffer* GetOutputBuffer()
	{ return &output_buffer_; }
private:
	UdpServer* server_;

	InetAddress peer_addr_;
	InetAddress dgram_addr_;

	Buffer input_buffer_;
	Buffer output_buffer_;

};
}
}

#endif //_UDPDGRAM_H_
