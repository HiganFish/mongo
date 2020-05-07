//
// Created by lsmg on 5/7/20.
//

#ifndef _TCPCLIENT_H_
#define _TCPCLIENT_H_

#include <map>
#include "Acceptor.h"
#include "ConnectionCallback.h"

namespace mongo
{
namespace net
{

class EventLoopThreadPoll;

class TcpClient : noncopyable
{
public:
	typedef std::function<void(const TcpConnectionPtr&)> NewConnectionCallback;

	TcpClient(EventLoop *loop, const std::string& name);

	/**
	 * start server listening
	 */
	void Start();

	void SetMessageCallback(const MessageCallback& cb)
	{ message_callback_ = cb; }

	void SetWriteOverCallback(const WriteOverCallback& cb)
	{ writeover_callback_ = cb; }

	void SetCloseCallback(const CloseCallback& cb)
	{ close_callback_ = cb; }

	void SetNewConnectionCallback(const NewConnectionCallback& cb)
	{ newconnection_callback_ = cb; }

	void SetThreadNum(int nums);

	void NewConnection(const char* addr, short port, void* arg = nullptr);
private:

	EventLoop* loop_;
	std::shared_ptr<EventLoopThreadPoll> poll_;
	std::string client_name_;
	InetAddress local_hosts_;

	MessageCallback message_callback_;
	WriteOverCallback writeover_callback_;
	CloseCallback close_callback_;
	NewConnectionCallback newconnection_callback_;

	typedef std::map<std::string, TcpConnectionPtr> ConnectionPtrMap;
	ConnectionPtrMap connections;

	void CloseConnection(const TcpConnectionPtr& conn);
	void DefaultMessageCallback(const TcpConnectionPtr& conn, Buffer* buffer);
};
}
}
#endif //_TCPCLIENT_H_
