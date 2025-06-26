#include <algorithm>
#include <string>
#include <unordered_map>
#include <slim/common/http/headers.h>
#include <slim/common/http/parser.h>
#include <slim/common/http/response.h>
#include <slim/common/http/request.h>
slim::common::http::Request::Request(void) {}
slim::common::http::Request::Request(char* request_pointer) {
	slim::common::http::parser::parse_http_request(request_pointer, this);
}
slim::common::http::Request::Request(std::string& request_string) {
	slim::common::http::parser::parse_http_request(request_string, this);
}
slim::common::http::Headers& slim::common::http::Request::headers(void) {
	return headers_map;
}
void slim::common::http::Request::method(std::string value) {
	method_string = value;
}
void slim::common::http::Request::method(std::string* value) {
	method_string = std::string(*value);
}
std::string& slim::common::http::Request::method(void) {
	return method_string;
}
void slim::common::http::Request::path(std::string value) {
	path_string = value;
}
void slim::common::http::Request::path(std::string* value) {
	path_string = std::string(*value);
}
std::string& slim::common::http::Request::path(void) {
	return path_string;
}
void slim::common::http::Request::version(std::string value) {
	version_string = value;
}
void slim::common::http::Request::version(std::string* value) {
	version_string = std::string(*value);
}
std::string& slim::common::http::Request::version(void) {
	return version_string;
}