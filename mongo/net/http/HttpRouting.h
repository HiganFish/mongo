//
// Created by lsmg on 6/5/20.
//

#ifndef _HTTPROUTING_H_
#define _HTTPROUTING_H_

#include <functional>
#include <memory>
#include <map>

#include "mongo/net/http/HttpContext.h"

namespace mongo
{
namespace net
{
class HttpContext;
class HttpResponse;
class HttpRequest;

typedef std::shared_ptr<HttpContext> HttpContextPtr;
typedef std::shared_ptr<HttpResponse> HttpResponsePtr;
typedef std::function<void(const HttpRequest& request, const HttpResponsePtr& response)> HttpRequestCallback;

class HttpRouting
{
public:
	HttpRouting();
	~HttpRouting();

	void SetDefaultMessageCallback(const HttpRequestCallback& callback)
	{ default_request_callback_ = callback; }

	void SetUrlForAdd(const std::string& url)
	{ url_temp_ = url; }

	void OnGet(const HttpRequestCallback& callback);
	void OnPost(const HttpRequestCallback& callback);
	void OnPut(const HttpRequestCallback& callback);
	void OnDelete(const HttpRequestCallback& callback);
	void OnUnknow(const HttpRequestCallback& callback);

	const HttpRequestCallback& operator[](const HttpRequest& request) const
	{
		return GetRequestCallback(request);
	}

private:

	HttpRequestCallback default_request_callback_;

	std::string url_temp_;

	struct Route
	{
		HttpRequestCallback on_get;
		HttpRequestCallback on_post;
		HttpRequestCallback on_put;
		HttpRequestCallback on_delete;
		HttpRequestCallback on_unknow;
	};

	typedef std::map<std::string, Route> RouteMap;
	RouteMap route_map_;

	const HttpRequestCallback& FindCallbackInMap(const Route& route, HttpRequest::EHttpMethod method) const;

	const HttpRequestCallback& GetRequestCallback(const HttpRequest& request) const;
};

}
}
#endif //_HTTPROUTING_H_
