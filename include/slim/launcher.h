#ifndef __SLIM__LAUNCHER__H
#define __SLIM__LAUNCHER__H
#include <string>
#include <vector>
namespace slim::launcher {
	void initialize(std::vector<std::string> v8_configuration_values);
	void launch(std::string script_name_string);
	void tear_down();
}
#endif