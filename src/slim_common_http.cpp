#include <algorithm>
#include <string>
#include <unordered_map>
#include <slim/common/http/headers.h>
#include <slim/common/http/parser.h>
#include <slim/common/http/response.h>
#include <slim/common/http/request.h>

#include <iostream>


/* slim::common::http::Headers */
std::unordered_map<std::string, std::string>& slim::common::http::Headers::get(void) {
	return headers;
}
void slim::common::http::Headers::set(std::string key, std::string value) {
	headers[key] = value;
}
slim::common::http::Headers& slim::common::http::Response::headers(void) {
	return headers_map;
}
/* tendril::http::Response */
void slim::common::http::Response::version(std::string value) {
	version_string = value;
}
void slim::common::http::Response::version(std::string* value) {
	version_string = std::string(*value);
}
std::string& slim::common::http::Response::version(void) {
	return version_string;
}
void slim::common::http::Response::body(std::string value) {
	body_string = value;
}
void slim::common::http::Response::body(std::string* value) {
	body_string = std::string(*value);
}
std::string& slim::common::http::Response::body(void) {
	return body_string;
}
void slim::common::http::Response::response_code(int value) {
	response_code_int = value;
	switch(response_code_int) {
		case 200: response_code_string_value = "OK"; break;
		case 404: response_code_string_value = "Not Found"; break;
		case 501: response_code_string_value = "Not Implemented"; break;
		case 505: response_code_string_value = "HTTP Version Not Supported"; break;
	}
}
int slim::common::http::Response::response_code(void) {
	return response_code_int;
}
std::string& slim::common::http::Response::response_code_string(void) {
	return response_code_string_value;
}
/* slim::common::http::Request */
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