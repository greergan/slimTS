#ifndef __SLIM__COMMON__HTTP__REQUEST__H
#define __SLIM__COMMON__HTTP__REQUEST__H
#include <slim/common/http/headers.h>
namespace slim::common::http {
	class Request {
		std::string method_string;
		std::string path_string;
		std::string version_string;
		Headers headers_map;
		public:
			Request(void);
			Request(char* request_pointer);
			Request(std::string& request_string);
			slim::common::http::Headers& headers(void);
			void path(std::string value);
			void path(std::string* value);
			std::string& path(void);
			void method(std::string value);
			void method(std::string* value);
			std::string& method(void);
			void version(std::string value);
			void version(std::string* value);
			std::string& version(void);
	};
};
#endif