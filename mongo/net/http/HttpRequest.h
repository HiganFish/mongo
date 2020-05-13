//
// Created by lsmg on 5/12/20.
//

#ifndef _HTTPREQUEST_H_
#define _HTTPREQUEST_H_

#include <map>
#include "mongo/base/Timestamp.h"

namespace mongo
{
namespace net
{

class HttpRequest
{
public:
	enum EHttpVersion
	{
		HTTP10,
		HTTP11,
		VERSION_UNKNOW
	};

	enum EHttpMethod
	{
		GET,
		POST,
		PUT,
		DELETE,
		METHOD_UNKNOW
	};

	HttpRequest();
	~HttpRequest();


	/*
	 * LEARN 下面使用了不少的 start和end来assign字符串 非常值得学习了. 很早前就知道可以这样, 但这样做是第一次
	 * */

	EHttpMethod GetMethod() const;
	void SetMethod(const char* start, const char* end);

	const std::string& GetPath() const;
	void SetPath(const char* start, const char* end);

	const std::string& GetQuery() const;
	void SetQuery(const char* start, const char* end);

	const Timestamp& GetRecvTime() const;
	void SetRecvTime(const Timestamp& recvTime);

	std::string GetHeader(const std::string& key) const;
	void AddHeader(const char* start, const char* end);

	EHttpVersion GetVersion() const;
	void SetVersion(const char* start, const char* end);

	void Reset();

private:

	const char HEADER_SPERATOR = ':';

	EHttpVersion version_;
	EHttpMethod method_;

	std::string path_;
	std::string query_;

	Timestamp recv_time_;

	typedef std::map<std::string, std::string> HeaderMap;
	HeaderMap headers_;
};
}
}

#endif //_HTTPREQUEST_H_
