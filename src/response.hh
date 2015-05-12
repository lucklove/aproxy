#pragma once

#include <string>
#include <vector>
#include <sstream>
#include "header.hh"
#include "package.hh"
#include "ptrs.hh"

class Response : public Package, 
	public std::enable_shared_from_this<Response> {
public:
	typedef unsigned short status_t;
	enum {
		Continue = 100, Switching, Processing,
		Ok = 200, Created, Accepted, Non_Authoritative_Information, 
			No_Content, Reset_Content, Partial_Content, Muti_Status,
		Multiple_Choices = 300, Moved_Permanently, Moved_Temporarily, See_Other, Not_Modified, 
		Bad_Request = 400, Unauthorized, Payment_Required, Forbidden, Not_Found,
		Internal_Server_Error = 500, Not_Implemented, Bad_Gateway, Service_Unavailable 
	};

	Response(ConnectionPtr connection) :
		Package(connection), status_(Ok) {}
	~Response() override;

	std::string getVersion() override { return version_; }
	void setVersion(const std::string& version) { version_ = version; }
	status_t getStatus() { return status_; }
	void setStatus(status_t status) { status_ = status; }
	std::string getMessage() { return msg_; }
	void setMessage(const std::string& msg) { msg_ = msg; }
private:
	std::string version_;
	status_t status_;
	std::string msg_;
};
