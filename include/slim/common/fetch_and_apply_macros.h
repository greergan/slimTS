#ifndef __SLIM__COMMON_FETCH_AND_PROCESS__H
#define __SLIM__COMMON_FETCH_AND_PROCESS__H
#include <string>
namespace slim::common {
	std::string fetch_and_apply_macros(const char* file_name_string);
}
#endif