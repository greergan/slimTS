#include <filesystem>
#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <slim/command_line_handler.h>
#include <slim/common/log.h>
#include <slim/common/memory_mapper.h>
#include <slim/path.h>
namespace slim::command_line {
	using namespace slim::common;
	std::unordered_map<std::string, std::string> slim_configuration_values;
	std::vector<std::string> script_configuration_values;
	std::vector<std::string> v8_configuration_values;
	std::unordered_set<std::string> allowed_file_extensions{".js",".mjs",".ts"};
	std::unordered_set<std::string> possible_slim_command_line_arguments
		{"--print-all", "--print-debug", "--print-error", "--print-info", "--print-trace"};
	std::unordered_map<std::string, bool> slim_command_line_argument_has_value;
	std::unordered_map<std::string, std::string> typescript_configurations
		{{"--print-typescript-all","false"},{"--print-typescript-debug","false"},{"--print-typescript-error","false"},
		{"--print-typescript-info","false"},{"--print-typescript-log","false"},{"--print-typescript-trace","false"},
		{"--print-typescript-warn","false"},{"--print-typescript-configuration","false"}};
	std::unordered_map<std::string, bool> typescript_command_line_argument_has_value;
}
const std::string& slim::command_line::get_script_name() {
	return slim_configuration_values["script_name"];
}
std::vector<std::string> slim::command_line::set_argv(int argc, char *argv[]) {
	log::trace(log::Message("slim::command_line::set_argv()","begins",__FILE__, __LINE__));
	slim_configuration_values["slim_executable"] = slim::path::getExecutablePath();
	v8_configuration_values.push_back(slim::path::getExecutablePath());
	script_configuration_values.push_back(slim_configuration_values["slim_executable"]);
	//slim_configuration_values["script_name"] = std::string("");
	bool done_searching_for_v8_arguments = false;
	bool done_searching_for_slim_arguments = false;
	bool found_module_source_file = false;
	try {
		for(int index = 1; index < argc; index++) {
			auto argument = std::string(argv[index]);
			if(done_searching_for_v8_arguments && done_searching_for_slim_arguments) {
				script_configuration_values.push_back(argument);
			}
			else if(typescript_configurations.contains(argument)) {
				if(typescript_command_line_argument_has_value[argument]) {
					typescript_configurations[argument] = argv[++index];
				}
				else {
					if(argument == "--print-typescript-all") {
						typescript_configurations[argument] = "true";
					}
					else if(argument == "--print-typescript-debug") {
						typescript_configurations[argument] = "true";
					}
					else if(argument == "--print-typescript-error") {
						typescript_configurations[argument] = "true";
					}
					else if(argument == "--print-typescript-info") {
						typescript_configurations[argument] = "true";
					}
					else if(argument == "--print-typescript-log") {
						typescript_configurations[argument] = "true";
					}
					else if(argument == "--print-typescript-trace") {
						typescript_configurations[argument] = "true";
					}
					else if(argument == "--print-typescript-warn") {
						typescript_configurations[argument] = "true";
					}
					else if(argument == "--print-typescript-configuration") {
						typescript_configurations[argument] = "true";
					}
				}
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
					else if(argument == "--print-typescript_warnings") {
						log::typescript_warning(true);
					}
				}			
			}
			else if(argument.find_last_of(".") >= 0) { // we found what looks like a file name so finish up
				found_module_source_file = true; // it might not be true but it is
				done_searching_for_slim_arguments = true;
				done_searching_for_v8_arguments = true;
				if(std::filesystem::exists(argument)) {
					auto script_path = std::filesystem::absolute(argument);
					if(allowed_file_extensions.contains(script_path.extension().string())) {
						auto script_name = std::regex_replace(script_path.string(), std::regex("/./"), "/");
						script_configuration_values.push_back(script_name);
						slim_configuration_values["script_name"] = script_name;
					}
				}
				else {
					throw "module file not found => " + argument;
				}
			}
			else {
				v8_configuration_values.push_back(std::string(argv[index]));
			}
		}
		if(!found_module_source_file) {
			//search for index.{js|ts|mjs}
			//throw "module file not specified";
		}
	}
	catch(const std::bad_alloc& e) {
		log::error("push_back failed");
		log::error(log::Message("slim::command_line::set_argv", "push_back failed" ,__FILE__, __LINE__));
	}
	std::string typescript_configuration_string("{");
	for(auto [key,value] : typescript_configurations) {
		std::string new_key;
		std::string new_value;
		if(key.starts_with("--")) {
			new_key = key.substr(2);
		}
		new_key = "\"" + new_key + "\":";
		if(value == "true" || value == "false") {
			new_value = value + ",";
		}
		else {
			new_value = "\"" + value + "\",";
		}
		typescript_configuration_string += new_key + new_value;
	}
	if(typescript_configuration_string.ends_with(",")) {
		typescript_configuration_string.pop_back();
	}
	typescript_configuration_string += "}";
	//log::info(typescript_configuration_string);
	memory_mapper::write("configurations", "typescript", std::make_shared<std::string>(typescript_configuration_string));
	log::trace(log::Message("slim::command_line::set_argv()","ends",__FILE__, __LINE__));
	return v8_configuration_values;
}
