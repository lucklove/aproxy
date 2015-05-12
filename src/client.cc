#include "client.hh"
#include "parser.hh"
#include "response.hh"
#include "request.hh"
#include "TcpConnection.hh"
#include "utils.hh"
#include <regex>
#include <boost/algorithm/string.hpp> 
#include <cstdio>

Client::Client(boost::asio::io_service& service)
 	:service_(service) 
{
}
	
void
Client::request(const std::string& method, const std::string& url,
	std::function<void(ResponsePtr)> res_handler,
	std::function<void(RequestPtr)> req_handler)
{
	/** http://user:pass@server:port/path?query */
	static const std::regex url_reg("(((http|https)://))?((((?!@).)*)@)?"
		"(((?![/\\?]).)+)(.+)?", std::regex::icase);	
	std::smatch results;
	if(std::regex_search(url, results, url_reg)) {
		std::string scheme = "http";

		if(results[3].matched) {
			scheme = results.str(3);
			boost::to_lower(scheme);
		}

		std::string host = results.str(7);

		std::string path = "/";
		if(results[9].matched)
			path = results.str(9);

		if(path[0] != '/')
			path = "/" + path;

		std::string port = scheme;
		static const std::regex host_port_reg("(((?!:).)*)(:([0-9]+))?");
		if(std::regex_search(host, results, host_port_reg)) {
			host = results.str(1);
			if(results[4].matched)
				port = results.str(4);
		}
		ConnectionPtr connection;
		connection = std::make_shared<TcpConnection>(service_);

		connection->async_connect(host, port, [=](ConnectionPtr conn) {
			if(conn) {
				auto req = std::make_shared<Request>(conn);
				auto res = std::make_shared<Response>(conn);
				req->setMethod(method);
				auto pos = path.find("?");
				if(pos == path.npos) {
					req->setPath(path);
				} else {
					req->setPath(path.substr(0, pos));
					req->setQueryString(path.substr(pos + 1, path.size()));
				}
				req->setVersion("HTTP/1.1");

				req_handler(req);
				if(!req->getHeader("Host"))
					req->addHeader("Host", host);
				/**
 				 * \note 
 				 * 	Client暂时没有保持连接的能力，不要使用keep-alive,
 				 * 	否则会给对端服务器造成不必要的麻烦
 				 */ 	
				req->setHeader("Connection", "close");

				parseResponse(res, [=](ResponsePtr response) {
					res->discardConnection();
					if(response) {
						res_handler(response);
					} else {
						res_handler(nullptr);
					}
				});
			} else {
				req_handler(nullptr);
				res_handler(nullptr);
			}
		});
	} else {
		res_handler(nullptr);
	}
}
