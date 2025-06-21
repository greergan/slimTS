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
	std::string script_argvs;
	std::unordered_map<std::string, std::string> slim_configuration_values;
	std::vector<std::string> v8_configuration_values;
	std::unordered_set<std::string> allowed_file_extensions{".js",".mjs",".ts"};
	std::unordered_set<std::string> slim_configurations
		{"--print-all","--print-debug","--print-error", "--print-info", "--print-trace","--print-thread-identity"
		,"--print-without-thread-identity"};
	std::unordered_map<std::string, bool> slim_command_line_argument_expects_value;
	std::unordered_map<std::string, std::string> typescript_configurations
		{{"--print-typescript-all","false"},{"--print-typescript-debug","false"},{"--print-typescript-error","false"},
		{"--print-typescript-info","false"},{"--print-typescript-log","false"},{"--print-typescript-trace","false"},
		{"--print-typescript-warn","false"},{"--print-typescript-configuration","false"},
		{"--typescript-project",""}};
	std::unordered_map<std::string, bool> typescript_command_line_argument_expects_value{
		{"--typescript-project",true}
	};
	auto is_script = [](std::string& argument)->std::string {
		log::trace(log::Message("slim::command_line::is_script()",std::string("begins =>" + argument).c_str(), __FILE__,__LINE__));
		auto argument_string = std::regex_replace(argument, std::regex("/./|//"), "/");
		std::string script_name_string;
		auto script_path = std::filesystem::absolute(argument_string);
		if(std::filesystem::exists(script_path)) {
			if(allowed_file_extensions.contains(script_path.extension().string())) {
				script_name_string = script_path.string();
				script_argvs +=  script_name_string + ",";
				slim_configuration_values["script_name"] = script_name_string;
			}
		}
		return script_name_string;
	};
	auto is_slim_argument = [](std::string& argument)->bool {
		log::trace(log::Message("slim::command_line::is_slim_argument()",std::string("begins =>" + argument).c_str(), __FILE__,__LINE__));
		return slim_configurations.find(argument) != slim_configurations.end();
	};
	auto slim_argument_expects_value = [](std::string& argument)->bool {
		log::trace(log::Message("slim::command_line::typescript_argument_expects_value()",std::string("begins =>" + argument).c_str(), __FILE__,__LINE__));
		return slim_command_line_argument_expects_value.find(argument) != slim_command_line_argument_expects_value.end() 
			&& slim_command_line_argument_expects_value.find(argument)->second;
	};
	auto is_typescript_argument = [](std::string& argument)->bool {
		log::trace(log::Message("slim::command_line::is_typescript_argument()",std::string("begins =>" + argument).c_str(), __FILE__,__LINE__));
		return typescript_configurations.find(argument) != typescript_configurations.end();
	};
	auto typescript_argument_expects_value = [](std::string& argument)->bool {
		log::trace(log::Message("slim::command_line::typescript_argument_expects_value()",std::string("begins =>" + argument).c_str(), __FILE__,__LINE__));
		return typescript_command_line_argument_expects_value.find(argument) != typescript_command_line_argument_expects_value.end() 
			&& typescript_command_line_argument_expects_value.find(argument)->second;
	};
}
const std::string& slim::command_line::get_script_name() {
	return slim_configuration_values["script_name"];
}
std::vector<std::string> slim::command_line::set_argv(int argc, char *argv[]) {
	log::trace(log::Message("slim::command_line::set_argv()","begins",__FILE__, __LINE__));
	slim_configuration_values["slim_executable"] = slim::path::getExecutablePath();
	v8_configuration_values.push_back(slim::path::getExecutablePath());
	script_argvs = slim_configuration_values["slim_executable"] + ",";
	bool found_module_source_file = false;
	try {
		for(int index = 1; index < argc; index++) {
			auto argument = std::string(argv[index]);
			if(found_module_source_file) {
				log::debug(log::Message("slim::command_line::set_argv()",std::string("argument => " + argument).c_str(),__FILE__,__LINE__));
				script_argvs += argument + ",";
			}
			else if(is_typescript_argument(argument)) {
				if(argument.starts_with("--print")) {
					typescript_configurations[argument] = "true";
				}
				else if(typescript_argument_expects_value(argument)) {
					typescript_configurations[argument] = std::string(argv[++index]);
				}
			}
			else if(is_slim_argument(argument)) {
				if(argument == "--print-all") {
					log::print_all(true);
				}
				else if(argument == "--print-debug") {
					log::print_debug(true);
				}
				else if(argument == "--print-error") {
					log::print_error(true);
				}
				else if(argument == "--print-info") {
					log::print_info(true);
				}
				else if(argument == "--print-thread-identity") {
					log::print_thread_identity(true);
				}
				else if(argument == "--print-trace") {
					log::print_trace(true);
				}
				else if(argument == "--print-without-thread-identity") {
					log::print_thread_identity(false);
				}
				else if(slim_argument_expects_value(argument)) {
					slim_configuration_values[argument] = argv[++index];
				}
			}
			else {
				auto script_file = is_script(argument);
				if(!script_file.empty()) {
					found_module_source_file = true;
				}
				else {
					v8_configuration_values.push_back(std::string(argv[index]));
				}
			}
		}
		if(!found_module_source_file) {
			//search for index.{js|ts|mjs}
			//throw "module file not specified";
		}
	}
	catch(const std::bad_alloc& e) {
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
	memory_mapper::write("configurations", "typescript", std::make_shared<std::string>(typescript_configuration_string));
	if(script_argvs.ends_with(",")) {
		script_argvs.pop_back();
	}
	memory_mapper::write("configurations", "script.argv", std::make_shared<std::string>(script_argvs));
	log::debug(log::Message("slim::command_line::set_argv()",std::string("typescript arguments => " + typescript_configuration_string).c_str(),__FILE__,__LINE__));
	log::debug(log::Message("slim::command_line::set_argv()",std::string("script arguments => " + script_argvs).c_str(),__FILE__,__LINE__));
	log::trace(log::Message("slim::command_line::set_argv()","ends",__FILE__, __LINE__));
	return v8_configuration_values;
}
