//
// Created by lsmg on 6/5/20.
//

#include "mongo/net/http/HttpRouting.h"
#include "mongo/net/http/HttpResponse.h"
using namespace mongo;
using namespace mongo::net;

HttpRouting::HttpRouting()
{

}
HttpRouting::~HttpRouting()
{

}
void HttpRouting::OnGet(const HttpRequestCallback& callback)
{
	route_map_[url_temp_].on_get = callback;
}

void HttpRouting::OnPost(const HttpRequestCallback& callback)
{
	route_map_[url_temp_].on_post = callback;
}

void HttpRouting::OnPut(const HttpRequestCallback& callback)
{
	route_map_[url_temp_].on_put = callback;
}

void HttpRouting::OnDelete(const HttpRequestCallback& callback)
{
	route_map_[url_temp_].on_delete = callback;
}

void HttpRouting::OnUnknow(const HttpRequestCallback& callback)
{
	route_map_[url_temp_].on_unknow = callback;
}


const HttpRequestCallback& HttpRouting::GetRequestCallback(const HttpRequest& request) const
{
	RouteMap::const_iterator iter = route_map_.find(request.GetPath());
	if (iter != route_map_.end())
	{
		const HttpRequestCallback& result = FindCallbackInMap(iter->second, request.GetMethod());
		if (result)
		{
			return result;
		}
	}

	return default_request_callback_;
}

const HttpRequestCallback&
HttpRouting::FindCallbackInMap(const HttpRouting::Route& route, HttpRequest::EHttpMethod method) const
{
	switch (method)
	{
	case HttpRequest::GET:
		return route.on_get;
	case HttpRequest::POST:
		return route.on_post;
	case HttpRequest::PUT:
		return route.on_put;
	case HttpRequest::DELETE:
		return route.on_delete;
	case HttpRequest::METHOD_UNKNOW:
		return route.on_unknow;
	}
}
