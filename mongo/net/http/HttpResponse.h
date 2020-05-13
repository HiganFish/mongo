//
// Created by lsmg on 5/12/20.
//

#ifndef _HTTPRESPONSE_H_
#define _HTTPRESPONSE_H_

#include <string>
#include <map>
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
		NOTFOUNT_404 = 400
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

	const std::string& GetResponseBody() const;
	void SetResponseBody(const std::string& responseBody);

	const char* ResponseCodeToString;

	void SetContextType(ContentType type);

	HttpResponse& operator[](const std::string& key);
	HttpResponse& operator=(const std::string& value);

private:
	ResponseCode response_code_;
	std::string response_message_;

	std::string response_body_;

	typedef std::map<std::string, std::string> HeaderMap;
	HeaderMap headers_;

	bool connecion_close_;

	std::string temp_key_;
};
}
}

#endif //_HTTPRESPONSE_H_
