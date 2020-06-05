//
// Created by lsmg on 5/12/20.
//

#ifndef _HTTPRESPONSE_H_
#define _HTTPRESPONSE_H_

#include <string>
#include <map>
#include <mongo/net/Buffer.h>
namespace mongo
{
namespace net
{

class Buffer;
class HttpResponse
{
public:

	/*
	 * LEARN 直接赋值 回应码极大方便了Encode
	 * */
	enum ResponseCode
	{
		OK200 = 200,
		NOTFOUNT_404 = 404
	};

	enum ContentType
	{
		TEXT_HTML
	};

	explicit HttpResponse(bool connecion_close);
	~HttpResponse();

	void EncodeToBuffer(Buffer* buffer);

	void AddHeader(const std::string& key, const std::string& value);

	ResponseCode GetResponseCode() const;
	void SetResponseCode(ResponseCode responseCode);

	const std::string& GetResponseMessage() const;
	void SetResponseMessage(const std::string& responseMessage);

	const Buffer& GetResponseBody() const;
	void SetResponseBody(const std::string& responseBody);

	bool SetBodyFilePath(const std::string& path);

	const char* ResponseCodeToString;

	void SetContextType(ContentType type);

	HttpResponse& operator[](const std::string& key);
	HttpResponse& operator=(const std::string& value);

	/**
	 * 供服务器内使用, 处理用户设置的返回body文件
	 * @param buffer
	 * @return true 文件读取完毕 false 文件未读取完毕
	 */
	bool ReadBodyToBuffer(Buffer* buffer);

	bool HasFileBody()
	{ return file_fd_ != -1; }

	bool IsCloseConnection()
	{ return connecion_close_;}
private:
	ResponseCode response_code_;
	std::string response_message_;

	int file_fd_;
	size_t read_bytes_;
	size_t sum_bytes_;

	Buffer response_body_;

	typedef std::map<std::string, std::string> HeaderMap;
	HeaderMap headers_;

	bool connecion_close_;

	std::string temp_key_;

	std::string DecodeUrl(const std::string& url);
	unsigned char FromHex(unsigned char x);
};
}
}

#endif //_HTTPRESPONSE_H_
