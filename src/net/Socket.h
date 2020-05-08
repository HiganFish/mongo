//
// Created by lsmg on 4/6/20.
//

#ifndef MONGO_SRC_NET_SOCKET_H
#define MONGO_SRC_NET_SOCKET_H

#include <netinet/in.h>

#include "InetAddress.h"
#include "noncopyable.h"
namespace mongo
{
namespace net
{
class Buffer;
class InetAddress;
class Socket : noncopyable
{
public:
	Socket();
    explicit Socket(int fd);
    ~Socket();

    void SetReuseAddr(bool enable);
    void SetReusePort(bool enable);

    void SetKeepAlive(bool enable);

    void Bind(const sockaddr_in& addr);
    void Bind(const InetAddress& addr);
    void Listening();

    int Accept(InetAddress* addr);

	ssize_t Send(const char* msg, size_t len);

	ssize_t Recv(Buffer* buffer);

	ssize_t SendTo(const char* msg, size_t len, const InetAddress& address);

    int GetFd() const
    { return sockfd_; }

	void SetConnectTimeout(int sec);

	void SetRecvTimeout(int sec);
private:
    int sockfd_;
};
}

namespace sockets
{
int CreateBlockFd();
int CreateNonBlockFd();
int CreateUdpFd();
void SetNonblocking(int fd);

void SetConnectTimeout(int sockfd, int sec);
void SetRecvTimeout(int sockfd, int sec);

int Connect(const mongo::net::InetAddress& addr);
}
}

#endif //MONGO_SRC_NET_SOCKET_H
