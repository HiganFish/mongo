//
// Created by lsmg on 5/12/20.
//

#include "HttpContext.h"

using namespace mongo;
using namespace mongo::net;

HttpContext::HttpContext():
status_(PARSE_FIRSTLINE)
{

}
HttpContext::~HttpContext()
{

}

bool HttpContext::Parse(Buffer* buffer)
{
	if (status_ == PARSE_FIRSTLINE)
	{
		if (!ParseFirstLine(buffer))
		{
			return false;
		}
	}
	if (status_ == PARSE_HEADERS)
	{
		if (!ParseHeaders(buffer))
		{
			return false;
		}
	}
	if (status_ == PARSE_BODY)
	{
		if (!ParseBody(buffer))
		{
			return false;
		}
	}

	return true;
}

bool HttpContext::ParseFirstLine(Buffer* buffer)
{
	const char* line_end = buffer->FindCrlf();
}
bool HttpContext::ParseHeaders(Buffer* buffer)
{
	return false;
}
bool HttpContext::ParseBody(Buffer* buffer)
{
	return false;
}
