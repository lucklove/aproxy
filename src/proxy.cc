/********************************************************************************
 *  ahttpd示例程序								*
 *  HTTP代理服务器								*
 *  by Joshua <gnu.crazier@gmail.com> at 2015-5-9				*
 ********************************************************************************/ 	

#include <iostream>
#include "server.hh"
#include "client.hh"
#include "TcpConnection.hh"
#include "proxy.hh"

namespace {
void
read_client(ConnectionPtr c, ConnectionPtr s)
{
	c->async_read(boost::asio::transfer_at_least(1), 
		[=](const boost::system::error_code& err, size_t n){
			if(err) {
				c->stop();
				s->stop();
				return;
			}
			std::stringstream ss;
			ss << &c->readBuffer();
			s->async_write(ss.str(), [=](const boost::system::error_code& e, size_t n) {
				if(e) {
					s->stop();
					c->stop();
				}
			});
			read_client(c, s);
		}
	);
}

void
read_server(ConnectionPtr c, ConnectionPtr s)
{
	s->async_read(boost::asio::transfer_at_least(1), 
		[=](const boost::system::error_code& err, size_t n){
			if(err) {
				c->stop();
				s->stop();
				return;
			}
			std::stringstream ss;
			ss << &s->readBuffer();
			c->async_write(ss.str(), [=](const boost::system::error_code& e, size_t n) {
				if(e) {
					c->stop();
					s->stop();
				}
			});
			read_server(c, s);
		}
	);
}

void
proxy_ssl(ConnectionPtr c, ConnectionPtr s)
{
	read_client(c, s);
	read_server(c, s);
}
}

void 
ProxyHandler::handleRequest(RequestPtr req, ResponsePtr res) 
{
//	if(req->proxyAuthInfo() != "luck:luck") {			/**< 用户:密码 */
//		res->setStatus(403);
//		return;
//	}
	std::string url = req->getPath();
	Log("NOTE") << "URL:" << url;

	if(req->getMethod() == "CONNECT") {				/**< ssl代理请求 */
		std::string server;
		std::string port = "https";							
		StringTokenizer st(url, ':');
		if(!st.hasMoreTokens()) {
			res->setStatus(400);
			return;
		}
		server = st.nextToken();
		if(st.hasMoreTokens())
			port = st.nextToken();
		ConnectionPtr s = std::make_shared<TcpConnection>(service_);
		s->async_connect(server, port, [=](ConnectionPtr conn) {
			if(!conn) {
				Log("ERROR") << "Connect to " << server << ":" << port << " failed";
				res->setStatus(500);
				return;
			}
			proxy_ssl(res->connection(), conn);
		});
	} else {
		if(req->getQueryString() != "")
			url += "?" + req->getQueryString();
		client_.request(req->getMethod(), url, 
			[=](ResponsePtr response) {
				if(!response)
					return;
				res->setStatus(response->getStatus());
				res->setMessage(response->getMessage());
				res->setVersion(response->getVersion());
				if(response->getHeader("Transfer-Encoding") && 
					*response->getHeader("Transfer-Encoding") == "chunked")
					response->delHeader("Transfer-Encoding");
				for(auto h : response->getHeaderMap())
					res->addHeader(h.name, h.value);
				if(response->in().rdbuf()->in_avail()) {
					std::stringstream ss;
					ss << response->in().rdbuf();
					res->out() << ss.str();
				}
			},
			[=](RequestPtr request) {
				if(!request)
					return;
				req->delHeader("Proxy-Connection");
				req->delHeader("Host");
				for(auto h : req->getHeaderMap())
					request->addHeader(h.name, h.value);
				if(req->in().rdbuf()->in_avail()) {		/**< 判断是否有数据, [重要]*/
					std::stringstream ss;
					ss << req->in().rdbuf();
					request->out() << ss.str();
				}
			}
		);
	}
}

int 
main(int argc, char* argv[])
{
	try {
		std::stringstream config("{\"http port\":\"8888\"}");
		Server server(config);						/**< 在8888端口监听 */
		server.addHandler("", new ProxyHandler(server.service()));	/**< 监听所有路径上的报文 */
		server.run(10);							/**< 给io_service 10个线程 */
	} catch(std::exception& e) {
		std::cerr << "exception: " << e.what() << "\n";
	}

	return 0;
}
