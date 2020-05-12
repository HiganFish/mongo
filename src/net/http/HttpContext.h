//
// Created by lsmg on 5/12/20.
//

#ifndef _HTTPCONTEXT_H_
#define _HTTPCONTEXT_H_

#include "Buffer.h"
#include "HttpRequest.h"

namespace mongo
{
namespace net
{
class HttpContext
{
	enum ParseStatus
	{
		PARSE_FIRSTLINE,
		PARSE_HEADERS,
		PARSE_BODY,
		PARSE_END
	};

	HttpContext();
	~HttpContext();

	bool Parse(Buffer* buffer);

	const HttpRequest& GetRequest() const
	{ return request_; }

	bool ParseOver() const
	{ return status_ == PARSE_END; }
private:

	ParseStatus status_;

	HttpRequest request_;

	bool ParseFirstLine(Buffer* buffer);

	bool ParseHeaders(Buffer* buffer);

	bool ParseBody(Buffer* buffer);
};
}
}

#endif //_HTTPCONTEXT_H_
