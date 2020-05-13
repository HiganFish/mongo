//
// Created by lsmg on 5/12/20.
//

#include <algorithm>
#include "HttpRequest.h"

using namespace mongo;
using namespace mongo::net;

HttpRequest::HttpRequest():
version_(VERSION_UNKNOW),
method_(METHOD_UNKNOW),
recv_time_()
{

}
HttpRequest::~HttpRequest()
{

}

HttpRequest::EHttpMethod HttpRequest::GetMethod() const
{
	return method_;
}
void HttpRequest::SetMethod(const char* start, const char* end)
{
	std::string method(start, end);

	/**
	 * LEARN 想之前为了效率 都是使用switch字符来选择. 后面想了下感觉为了
	 * 那点点性能复杂化逻辑是不是在其他地方值得优化的时候不值呢?
	 */
	if (method == "GET")
	{
		method_ = GET;
	}
	else if (method == "POST")
	{
		method_ = POST;
	}
	else if (method == "PUT")
	{
		method_ = PUT;
	}
	else if (method == "DELETE")
	{
		method_ = DELETE;
	}
}

const std::string& HttpRequest::GetPath() const
{
	return path_;
}
void HttpRequest::SetPath(const char* start, const char* end)
{
	path_.assign(start, end);
}

const std::string& HttpRequest::GetQuery() const
{
	return query_;
}
void HttpRequest::SetQuery(const char* start, const char* end)
{
	query_.assign(start, end);
}

const Timestamp& HttpRequest::GetRecvTime() const
{
	return recv_time_;
}
void HttpRequest::SetRecvTime(const Timestamp& recvTime)
{
	recv_time_ = recvTime;
}

std::string HttpRequest::GetHeader(const std::string& key) const
{
	std::string result;

	HeaderMap::const_iterator iter = headers_.find(key);

	if (iter != headers_.end())
	{
		result = iter->second;
	}
	return result;
}
void HttpRequest::AddHeader(const char* start, const char* end)
{
	const char* key_end = std::find(start, end, HEADER_SPERATOR);

	if ((!key_end) || (key_end == end))
	{
		return;
	}

	const char* value_start = key_end + 1;
	if (*value_start == ' ')
	{
		value_start++;
	}

	std::string key(start, key_end);
	std::string value(value_start, end);
	headers_.insert({key, value});
}
HttpRequest::EHttpVersion HttpRequest::GetVersion() const
{
	return version_;
}
void HttpRequest::SetVersion(const char* start, const char* end)
{
	std::string version(start, end);

	if (version == "HTTP/1.0")
	{
		version_ = HTTP10;
	}
	else if (version == "HTTP/1.1")
	{
		version_ = HTTP11;
	}
}
void HttpRequest::Reset()
{
	version_ = VERSION_UNKNOW;
	method_ = METHOD_UNKNOW;

	headers_.clear();
}
