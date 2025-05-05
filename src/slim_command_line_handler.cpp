#include <string>
#include <vector>
#include <slim/command_line_handler.h>
#include <slim/common/log.h>
namespace slim::command_line {
	std::vector<std::string> args;
}
std::vector<std::string> slim::command_line::get_argv(void) {
	return slim::command_line::args;
}
void slim::command_line::set_argv(int argc, char *argv[]) {
	slim::common::log::trace(slim::common::log::Message("slim::command_line::set_argv",std::string("command line arguments => " + std::to_string(argc)).c_str(),__FILE__, __LINE__));
	try {
		for(int index = 0; index < argc; index++) {
			slim::command_line::args.push_back(std::string(argv[index]));
			slim::common::log::debug(slim::common::log::Message("slim::command_line::set_argv",std::string("command line argument => " + std::string(argv[index])).c_str(),__FILE__, __LINE__));
			slim::common::log::debug(slim::common::log::Message("slim::command_line::set_argv",std::string("command line argument => " + std::to_string(index)).c_str(),__FILE__, __LINE__));
		}
	}
	catch(const std::bad_alloc& e) {
		slim::common::log::error("push_back failed");
		slim::common::log::error(slim::common::log::Message("slim::command_line::set_argv", "push_back failed" ,__FILE__, __LINE__));
	}
	slim::common::log::trace(slim::common::log::Message("slim::command_line::set_argv",std::string("command line arguments => " + std::to_string(argc)).c_str(),__FILE__, __LINE__));
}