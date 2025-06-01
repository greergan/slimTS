#include <filesystem>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <slim/command_line_handler.h>
#include <slim/common/log.h>
#include <slim/path.h>
namespace slim::command_line {
	using namespace slim::common;
	char** v8_configuration_values;
	static std::unordered_map<std::string, std::string> slim_configuration_values;
	static std::vector<std::string> script_configuration_values;
	static std::unordered_set<std::string> possible_slim_command_line_arguments
		{"--print-all", "--print-debug", "--print-error", "--print-info", "--print-trace"};
	static std::unordered_map<std::string, bool> slim_command_line_argument_has_value;
}
const std::string& slim::command_line::get_script_name() {
	return slim_configuration_values["script_name"];
}
char** slim::command_line::set_argv(int argc, char *argv[]) {
	log::trace(log::Message("slim::command_line::set_argv()","begins",__FILE__, __LINE__));
	slim_configuration_values["slim_executable"] = slim::path::getExecutablePath();
	script_configuration_values.push_back(slim_configuration_values["slim_executable"]);
	bool done_searching_for_v8_arguments = false;
	bool done_searching_for_slim_arguments = false;
	try {
		for(int index = 1; index < argc; index++) {
			auto argument = std::string(argv[index]);
			if(done_searching_for_v8_arguments && done_searching_for_slim_arguments) {
				script_configuration_values.push_back(argument);
			}
			else if(possible_slim_command_line_arguments.contains(argument)) {
				if(slim_command_line_argument_has_value[argument]) {
					index++;
					slim_configuration_values[argument] = argv[index];
				}
				else {
					if(argument == "--print-all") {
						log::all(true);
					}
					else if(argument == "--print-debug") {
						log::debug(true);
					}
					else if(argument == "--print-error") {
						log::error(true);
					}
					else if(argument == "--print-info") {
						log::info(true);
					}
					else if(argument == "--print-trace") {
						log::trace(true);
					}
				}			
			}
			else if(std::filesystem::exists(argument)) {
				done_searching_for_slim_arguments = true;
				done_searching_for_v8_arguments = true;
				auto script_name = std::filesystem::absolute(argument).string();
				script_configuration_values.push_back(script_name);
				slim_configuration_values["script_name"] = script_name;
			}
			else {
				v8_configuration_values[index] = argv[index];
			}
		}
	}
	catch(const std::bad_alloc& e) {
		log::error("push_back failed");
		log::error(log::Message("slim::command_line::set_argv", "push_back failed" ,__FILE__, __LINE__));
	}
	log::trace(log::Message("slim::command_line::set_argv()","ends",__FILE__, __LINE__));
	return v8_configuration_values;
}
