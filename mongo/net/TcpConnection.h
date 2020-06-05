//
// Created by lsmg on 4/7/20.
//

#ifndef MONGO_SRC_NET_TCPCONNECTION_H
#define MONGO_SRC_NET_TCPCONNECTION_H

#include <memory>
#include <map>
#include "mongo/base/noncopyable.h"
#include "mongo/base/Timestamp.h"
#include "mongo/net/ConnectionCallback.h"
#include "mongo/net/Buffer.h"
#include "mongo/net/InetAddress.h"

namespace mongo
{
namespace net
{
class EventLoop;
class Socket;
class Channel;

class TcpConnection : public noncopyable, public std::enable_shared_from_this<TcpConnection>
{
public:
    typedef std::function<void(const TcpConnectionPtr& conn, void* arg)> TimeOverCallback;

    TcpConnection(EventLoop* loop, const std::string& name, int sockfd, const InetAddress& host_addr, const InetAddress& client_addr);
    ~TcpConnection();

    void Send(const char* msg, size_t len);
    void Send(Buffer* buffer);

    void SetMessageCallback(const MessageCallback& cb)
    { message_callback_ = cb; }

    void SetWriteOverCallback(const WriteOverCallback& cb)
    { write_over_callback_ = cb; }

	void SetWritableCallback(const WritableCallback & cb)
	{ writable_callback_ = cb; }

    void SetCloseCallback(const CloseCallback& cb)
    { close_callback_ = cb; }

    const std::string& GetConnectionName() const
    { return connection_name_; }

    const InetAddress& GetClientAddr() const
    { return client_addr_; }

    void AddTimer(const TimeOverCallback& callback, void* arg, const std::string& key, int sec, int msec, bool repeat = false, int count = -1);

    void ConnectionCreated();

    void CloseConnection();

    bool Connected() const
    { return status_ == CONNECTED; }

    void EnableAutoClose(int sec);

    void* GetArg()
	{ return arg_; }

	void SetArg(void* arg)
	{
    	arg_ = arg;
	}

	void EnableWriting();

	void DisableWriting();
private:

	void* arg_;

    enum Status{CONNECTING, CONNECTED, CLOSEING, CLOSED};

    EventLoop* loop_;
    std::string connection_name_;
    Status status_;

    Buffer input_buffer_;
    Buffer output_buffer_;

    size_t sum_recv_;
    Timestamp established_time_;

    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;

    const InetAddress host_addr_;
    const InetAddress client_addr_;

    MessageCallback message_callback_;

    // 发送未发送完的部分 归TCPConnection所管理, 发送完毕后可以触发回调
    WriteOverCallback write_over_callback_;
    WriteOverCallback writable_callback_;
    CloseCallback close_callback_;

    void ReadHandle();
    void WriteHandle();
    void ErrorHandle();
    void CloseHandle();
    void TimerOverHandle(const std::string& key);
    void AutoCloseConnHandle();

    struct TimerCallbackFunc
    {
        TimeOverCallback callback;
        void* arg;
    };

    typedef std::map<std::string, TimerCallbackFunc> TimerCallbackMap;
    TimerCallbackMap timer_callback_map_;
};

}
}
#endif //MONGO_SRC_NET_TCPCONNECTION_H
