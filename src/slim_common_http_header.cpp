#include <string>
#include <unordered_map>
#include <slim/common/http/headers.h>
#include <slim/common/http/parser.h>
#include <slim/common/http/response.h>
#include <slim/common/http/request.h>
std::unordered_map<std::string, std::string>& slim::common::http::Headers::get(void) {
	return headers;
}
void slim::common::http::Headers::set(std::string key, std::string value) {
	headers[key] = value;
}
slim::common::http::Headers& slim::common::http::Response::headers(void) {
	return headers_map;
}
