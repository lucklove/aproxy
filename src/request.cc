#include "request.hh"
#include "connection.hh"
#include "utils.hh"
#include "base64.hh"
#include <regex>

namespace {
std::string
auth_info(const std::string auth)
{
	static const std::regex basic_auth_reg("[Bb]asic ([[:print:]]*)");
	std::smatch results;
	if(std::regex_search(auth, results, basic_auth_reg)) {
		return Base64::decode(results.str(1));
	} else {
		return {};
	}
}
}

std::string
Request::proxyAuthInfo()
{
	auto auth = getHeader("Proxy-Authorization"); 
	if(!auth) return {};
	return auth_info(*auth);
}

Request::~Request()
{

	if(connection() == nullptr)
		return;
	try {
		if(!chunked()) {
			if(query_ == "") {
				Log("NOTE") << method_ << " " << path_ << " " << version_;
				connection()->async_write(method_ + " " + path_ + " " + version_ + "\r\n");
			} else {
				Log("NOTE") << method_ << " " << path_ << "?" << query_ << " " << version_;
				connection()->async_write(method_ + " " + path_ + "?" + query_ + " " + version_ + "\r\n");
			}
		} else {
			flushPackage();
			connection()->async_write("0\r\n\r\n");
		}
		flushPackage();
	} catch(std::exception& e) {
		fprintf(stderr, "%s\n", e.what());
	}
}
		
