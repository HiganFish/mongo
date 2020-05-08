//
// Created by lsmg on 4/6/20.
//

#include <sys/socket.h>
#include <zconf.h>
#include <Logger.h>
#include <netdb.h>
#include <Endian.h>
#include "Socket.h"
#include <Buffer.h>

using namespace mongo;

net::Socket::Socket():
sockfd_(sockets::CreateBlockFd())
{

}

net::Socket::Socket(int fd):
    sockfd_(fd)
{
}
net::Socket::~Socket()
{
    close(sockfd_);
}

void net::Socket::SetReuseAddr(bool enable)
{
    int opt = enable ? 1 : 0;
    int ret = setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, static_cast<void*>(&opt), static_cast<socklen_t>(sizeof opt));

    LOG_FATAL_IF(ret == -1) << (enable ? "enable" : "disable") << " reuseaddr error";
}
void net::Socket::SetReusePort(bool enable)
{
    int opt = enable ? 1 : 0;
    int ret = setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, static_cast<void*>(&opt), static_cast<socklen_t>(sizeof opt));

    LOG_FATAL_IF(ret == -1) << (enable ? "enable" : "disable") << " reuseport error";
}

void net::Socket::SetKeepAlive(bool enable)
{
    int opt = enable ? 1 : 0;
    int ret = setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, static_cast<void*>(&opt), static_cast<socklen_t>(sizeof opt));

    LOG_FATAL_IF(ret == -1) << (enable ? "enable" : "disable") << " keepalive error";
}

void net::Socket::Bind(const net::InetAddress& addr)
{
	Bind(addr.GetAddr());
}

void net::Socket::Bind(const sockaddr_in& addr)
{
    int ret = bind(sockfd_, (sockaddr*)&addr, static_cast<socklen_t>(sizeof addr));
    LOG_FATAL_IF(ret == -1) << "bind error";
}
void net::Socket::Listening()
{
    int ret = listen(sockfd_, SOMAXCONN);
    LOG_FATAL_IF(ret == -1) << "listen error";
}
int net::Socket::Accept(InetAddress* addr)
{
    sockaddr_in address{};
    socklen_t socklen = static_cast<socklen_t>(sizeof address);
    int sockfd = accept(sockfd_, (sockaddr*)&address, &socklen);
    LOG_FATAL_IF(sockfd == -1) << "accept error";
    addr->SetAddr(address);

    return sockfd;
}
ssize_t net::Socket::Send(const char* msg, size_t len)
{
    ssize_t ret = send(sockfd_, msg, len, MSG_DONTWAIT);
    return ret;
}
ssize_t net::Socket::SendTo(const char* msg, size_t len, const net::InetAddress& address)
{
	ssize_t ret = sendto(sockfd_, msg, len,0 , address.GetAddrPtr(), address.GetAddrLen());

	return ret;
}

ssize_t net::Socket::Recv(net::Buffer* buffer)
{
	return buffer->ReadFromFd(sockfd_);
}
void net::Socket::SetConnectTimeout(int sec)
{
	sockets::SetConnectTimeout(sockfd_, sec);
}
void net::Socket::SetRecvTimeout(int sec)
{
	sockets::SetRecvTimeout(sockfd_, sec);
}

int sockets::CreateBlockFd()
{
	int sockfd =  socket(PF_INET, SOCK_STREAM, 0);
	LOG_FATAL_IF(sockfd < 0) << "socket create error";
}

int sockets::CreateNonBlockFd()
{
    int sockfd =  socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    LOG_FATAL_IF(sockfd < 0) << "socket create error";
}

int sockets::CreateUdpFd()
{
	int sockfd =  socket(PF_INET, SOCK_DGRAM, 0);
	LOG_FATAL_IF(sockfd < 0) << "socket create error";
}

void sockets::SetNonblocking(int fd)
{

}

void sockets::SetRecvTimeout(int sockfd, int sec)
{
	struct timeval timeout{sec, 0};

	int ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, static_cast<socklen_t>(sizeof timeout));
	LOG_FATAL_IF(ret < 0) << "socket set timeout error";
}

void sockets::SetConnectTimeout(int sockfd, int sec)
{
	struct timeval timeout{sec, 0};

	int ret = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, static_cast<socklen_t>(sizeof timeout));
	LOG_FATAL_IF(ret < 0) << "socket set timeout error";
}

int sockets::Connect(const mongo::net::InetAddress& server_addr)
{
	int sockfd = CreateBlockFd();
	sockets::SetConnectTimeout(sockfd, 1);

	const sockaddr_in& addr = server_addr.GetAddr();

	if (connect(sockfd, (sockaddr*)&addr, sizeof addr) == 0)
	{
		return sockfd;
	}

	close(sockfd);
	return -1;
}