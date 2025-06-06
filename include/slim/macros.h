#ifndef __SLIM__MACROS__H
#define __SLIM__MACROS__H
#include <memory>
#include <string>
#include <sstream>
namespace slim::macros {
	std::unique_ptr<std::string> apply(std::shared_ptr<std::string> content_pointer, const std::string& absolute_path_to_file);
	std::unique_ptr<std::stringstream> apply(std::unique_ptr<std::stringstream> input_stringstream_pointer, const std::string& absolute_path_to_file);
}
#endif