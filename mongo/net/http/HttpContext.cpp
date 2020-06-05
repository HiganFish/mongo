//
// Created by lsmg on 5/12/20.
//

#include <algorithm>
#include "mongo/net/http/HttpContext.h"

using namespace mongo;
using namespace mongo::net;

HttpContext::HttpContext():
status_(PARSE_FIRSTLINE)
{

}
HttpContext::~HttpContext()
{

}

bool HttpContext::Parse(Buffer* buffer, Timestamp recv_time)
{
	recv_time_ = recv_time;

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
	const char* line_begin = buffer->ReadBegin();
	const char* line_end = buffer->FindCrlf();
	if (line_end == nullptr)
	{
		return true;
	}
	buffer->AddReadIndex(line_end + 2 - line_begin);


	const char* method_end = std::find(line_begin, line_end, ' ');
	if (method_end == line_end)
	{
		return false;
	}
	request_.SetMethod(line_begin, method_end);

	method_end++;
	const char* path_end = std::find(method_end, line_end, ' ');
	if (path_end == line_end)
	{
		return false;
	}
	request_.SetPath(method_end, path_end);

	path_end++;
	request_.SetVersion(path_end, line_end);

	status_ = PARSE_HEADERS;
	return true;
}
bool HttpContext::ParseHeaders(Buffer* buffer)
{
	const char* line_begin = nullptr;
	const char* line_end = nullptr;

	while (true)
	{
		line_begin = buffer->ReadBegin();
		line_end = buffer->FindCrlf();
		if (line_end == nullptr)
		{
			return true;
		}
		buffer->AddReadIndex(line_end + 2 - line_begin);
		if (line_end == line_begin)
		{
			break;
		}
		request_.AddHeader(line_begin, line_end);
	}

	status_ = PARSE_BODY;
	return true;
}
bool HttpContext::ParseBody(Buffer* buffer)
{
	status_ = PARSE_END;
	return true;
}
void HttpContext::Reset()
{
	status_ = PARSE_FIRSTLINE;
	request_.Reset();
}
