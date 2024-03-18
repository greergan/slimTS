#ifndef __SLIM__NETWORK__HTTP__HEADERS__H
#define __SLIM__NETWORK__HTTP__HEADERS__H
#include <iostream>
#include <unordered_map>
//https://fetch.spec.whatwg.org/
//https://fetch.spec.whatwg.org/#headers-class
namespace slim::network::http {
	struct HeadersInit {};
	class Headers {
		std::unordered_map<std::string, std::string> header_map;
		public:
			Headers(HeadersInit init = {});
			void append_header(std::string_view name, std::string_view value);
			void delete_header(std::string_view name);
			std::string get_header(std::string_view name);
			std::string get_set_cookie(void);
			bool has_header(std::string_view name);
			void set_header(std::string_view name, std::string_view value);
	};
}
#endif