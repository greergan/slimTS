#ifndef __SLIM__COMMON__HTTP__PARSER__H
#define __SLIM__COMMON__HTTP__PARSER__H
#include <string>
#include <slim/common/http/request.h>
namespace slim::common::http::parser {
	static void parse_line_1(std::string& method_string, slim::common::http::Request* request);
	void parse_http_request(const char* request_pointer, slim::common::http::Request* request);
	void parse_http_request(std::string& request_string, slim::common::http::Request* request);
}
#endif