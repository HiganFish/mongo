//
// Created by lsmg on 5/12/20.
//

#include "HttpResponse.h"
#include "mongo/net/Buffer.h"

using namespace mongo;
using namespace mongo::net;

HttpResponse::HttpResponse(bool connecion_close):
connecion_close_(connecion_close),
temp_key_()
{

}
HttpResponse::~HttpResponse()
{

}
void HttpResponse::EncodeToBuffer(Buffer* buffer)
{
	char temp_buffer[40]{};

	int len = snprintf(temp_buffer, sizeof temp_buffer, "HTTP/1.1 %d ", static_cast<int>(response_code_));
	buffer->Append(temp_buffer, len);
	buffer->Append(response_message_);
	buffer->Append("\r\n");

	if (connecion_close_)
	{
		buffer->Append("Connection: close\r\n");
	}
	else
	{
		/**
		 * TODO  size length   c_str data  difference
		 */

		len = snprintf(temp_buffer, sizeof temp_buffer, "Content-Length: %zd\r\n", response_body_.size());
		buffer->Append(temp_buffer, len);
		buffer->Append("Connection: Keep-Alive\r\n");
	}

	for (const auto& iter : headers_)
	{
		buffer->Append(iter.first);
		buffer->Append(": ");
		buffer->Append(iter.second);
		buffer->Append("\r\n");
	}

	buffer->Append("\r\n");
	buffer->Append(response_body_);
}


HttpResponse::ResponseCode HttpResponse::GetResponseCode() const
{
	return response_code_;
}

void HttpResponse::SetResponseCode(HttpResponse::ResponseCode responseCode)
{
	response_code_ = responseCode;
}

const std::string& HttpResponse::GetResponseMessage() const
{
	return response_message_;
}

void HttpResponse::SetResponseMessage(const std::string& responseMessage)
{
	response_message_ = responseMessage;
}

const std::string& HttpResponse::GetResponseBody() const
{
	return response_body_;
}

void HttpResponse::SetResponseBody(const std::string& responseBody)
{
	response_body_ = responseBody;
}

void HttpResponse::AddHeader(const std::string& key, const std::string& value)
{
	headers_[key] = value;
}

void HttpResponse::SetContextType(HttpResponse::ContentType type)
{
	switch (type)
	{
	case TEXT_HTML:
		AddHeader("Content-Type", "text/html; charset=utf-8");
		break;
	}
}

HttpResponse& HttpResponse::operator[](const std::string& key)
{
	temp_key_ = key;
	return *this;
}

HttpResponse& HttpResponse::operator=(const std::string& value)
{
	AddHeader(temp_key_, value);
}

