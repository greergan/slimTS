#ifndef __SLIM__COMMON__UTILITIES__H
#define __SLIM__COMMON__UTILITIES__H
#include <string>
namespace slim::common::utilities {
	bool get_bool_value(char* value);
	int get_int_value(char* value);
	int get_int_value(std::string value);
	int get_int_value(std::string* value);
}
#endif