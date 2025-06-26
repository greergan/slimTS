#ifndef __SLIM__COMMON__HTTP__HEADERS__H
#define __SLIM__COMMON__HTTP__HEADERS__H
#include <string>
#include <unordered_map>
namespace slim::common::http {
	struct Headers {
		void set(std::string key, std::string value);
		std::unordered_map<std::string, std::string>& get(void);
		private:
			std::unordered_map<std::string, std::string> headers;
	};
};
#endif