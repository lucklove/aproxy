#pragma once

struct ProxyHandler : public RequestHandler {
	ProxyHandler(boost::asio::io_service& s) : service_(s), client_(s) {}

	void handleRequest(RequestPtr req, ResponsePtr res) override; 
private:
	boost::asio::io_service& service_;	
	Client client_;
};

