#ifndef __SLIM__MACROS__H
#define __SLIM__MACROS__H
#include <sstream>
namespace slim::macros {
	std::stringstream apply(std::stringstream& input_sting_stream, const std::string& file_name_string);
}
#endif