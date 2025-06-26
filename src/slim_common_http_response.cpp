#include <algorithm>
#include <string>
#include <unordered_map>
#include <slim/common/http/headers.h>
#include <slim/common/http/parser.h>
#include <slim/common/http/response.h>
#include <slim/common/http/request.h>
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
