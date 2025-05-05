#ifndef __SLIM__COMMAND_LINE_HANDLER__H
#define __SLIM__COMMAND_LINE_HANDLER__H
#include <vector>
namespace slim::command_line {
	void set_argv(int argc, char *argv[]);
	std::vector<std::string> get_argv(void);
}
#endif