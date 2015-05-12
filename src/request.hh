#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <sstream>
#include "package.hh"
#include "header.hh"
#include "log.hh"

/**
 * \breif 包含http client的请求信息
 */
class Request : public Package, 
	public std::enable_shared_from_this<Request> {
public :
	using Package::Package;
	~Request() override;
			
	/**
 	 * \brief 获取请求的path
 	 * \return path
 	 */ 
	std::string getPath() { return path_; }

 	/**
	 * \brief 设置path
 	 */ 
	void setPath(const std::string& path) { path_ = path; }


	/**
 	 * \brief 获取请求中的query string.
 	 * \return query string.
 	 */
	std::string getQueryString() { return query_; }

 	/** 
	 * \brief 设置请求中的query string.
 	 * \return query string.
 	 */
	void setQueryString(const std::string& query) { query_ = query; }
 
 
	/**
 	 * \brief 获取请求方法
 	 * \return method
 	 */ 
	std::string getMethod() { return method_; }

	/**
 	 * \brief 设置请求方法
 	 */ 
	void setMethod(const std::string& method) { method_ = method; }

	/**
 	 * \brief 获取http版本HTTP/1.0 HTTP1.1
 	 * \return 版本信息
 	 */  
	std::string getVersion() override { return version_; }
	
	/**
 	 * \brief 设置http版本HTTP/1.0 HTTP1.1
 	 */  
	void setVersion(const std::string& version) { version_ = version; }

	std::string proxyAuthInfo();

private:
	std::string method_;
	std::string path_;
	std::string query_;
	std::string version_;
};
