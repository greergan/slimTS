#ifndef __SLIM__MACROS__H
#define __SLIM__MACROS__H
#include <sstream>
namespace slim::macros {
	std::string apply(const std::string& file_contents, const std::string& absolute_path_to_file);
	std::string apply(std::stringstream& input_sting_stream, const std::string& file_name_string);
}
#endif