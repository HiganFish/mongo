//
// Created by lsmg on 5/12/20.
//

#ifndef _HTTPCONTEXT_H_
#define _HTTPCONTEXT_H_

#include "mongo/net/Buffer.h"
#include "mongo/net/http/HttpRequest.h"

namespace mongo
{
namespace net
{
class HttpContext
{
public:
	enum ParseStatus
	{
		PARSE_FIRSTLINE,
		PARSE_HEADERS,
		PARSE_BODY,
		PARSE_END
	};

	HttpContext();
	~HttpContext();

	bool Parse(Buffer* buffer, Timestamp recv_time);

	const HttpRequest& GetRequest() const
	{ return request_; }

	bool ParseOver() const
	{ return status_ == PARSE_END; }

	void Reset();
private:

	ParseStatus status_;

	HttpRequest request_;

	Timestamp recv_time_;

	bool ParseFirstLine(Buffer* buffer);

	bool ParseHeaders(Buffer* buffer);

	bool ParseBody(Buffer* buffer);
};
}
}

#endif //_HTTPCONTEXT_H_
