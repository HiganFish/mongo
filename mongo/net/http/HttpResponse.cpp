//
// Created by lsmg on 5/12/20.
//

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "HttpResponse.h"
#include "mongo/base/Logger.h"
#include "mongo/net/Buffer.h"

using namespace mongo;
using namespace mongo::net;

HttpResponse::HttpResponse(bool connecion_close):
connecion_close_(connecion_close),
temp_key_(),
file_fd_(-1),
read_bytes_(0),
sum_bytes_(0)
{

}
HttpResponse::~HttpResponse()
{
	if (file_fd_ != -1)
	{
		close(file_fd_);
	}
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

		len = snprintf(temp_buffer, sizeof temp_buffer, "Content-Length: %zd\r\n", response_body_.ReadableBytes() + sum_bytes_);
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
	buffer->Append(&response_body_);
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

const Buffer& HttpResponse::GetResponseBody() const
{
	return response_body_;
}

void HttpResponse::SetResponseBody(const std::string& responseBody)
{
	response_body_.Append(responseBody);
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
bool HttpResponse::SetBodyFilePath(const std::string& path)
{
	std::string file_path = DecodeUrl(path);
	struct stat file_stat;

	int result = stat(file_path.c_str(), &file_stat);
	if (S_ISDIR(file_stat.st_mode))
	{
		file_path.append("/index.html");
		result = stat(file_path.c_str(), &file_stat);
	}
	if (result == -1)
	{
		LOG_WARN << "Open path " << file_path << " failure errormsg: " << strerror(errno);
		return false;
	}

	file_fd_ = open(file_path.c_str(), O_RDONLY);
	if (file_fd_ == -1)
	{
		LOG_WARN << "Open fd " << path << " failure errormsg: " << strerror(errno);
		return false;
	}
	sum_bytes_ = file_stat.st_size;

	return true;
}
std::string HttpResponse::DecodeUrl(const std::string& url)
{
	std::string strTemp;
	size_t length = url.length();
	for (size_t i = 0; i < length; i++)
	{
		if (url[i] == '+')
		{
			strTemp += ' ';
		}
		else if (url[i] == '%')
		{
			assert(i + 2 < length);
			unsigned char high = FromHex((unsigned char)url[++i]);
			unsigned char low = FromHex((unsigned char)url[++i]);
			strTemp.append(1, high * 16 + low);
		}
		else
		{
			strTemp += url[i];
		}
	}
	return strTemp;
}
unsigned char HttpResponse::FromHex(unsigned char x)
{
	unsigned char y;
	if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
	else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
	else if (x >= '0' && x <= '9') y = x - '0';
	else assert(0);
	return y;
}
bool HttpResponse::ReadBodyToBuffer(Buffer* buffer)
{
	if (!HasFileBody())
	{
		return true;
	}
	buffer->DropAllData();

	size_t read_bytes = buffer->ReadFromFd(file_fd_);
	if (read_bytes < 0)
	{
		return true;
	}

	read_bytes_ += read_bytes;

	return read_bytes_ == sum_bytes_;
}

