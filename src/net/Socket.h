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
class InetAddress;
class Socket : noncopyable
{
public:
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

	ssize_t SendTo(const char* msg, size_t len, const InetAddress& address);

    int GetFd() const
    { return sockfd_; }


private:
    int sockfd_;
};
}

namespace sockets
{
int CreateNonBlockFd();
int CreateUdpFd();
void SetNonblocking(int fd);
}
}

#endif //MONGO_SRC_NET_SOCKET_H
