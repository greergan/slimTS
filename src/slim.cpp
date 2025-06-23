#include <filesystem>
#include <future>
#include <string>
#include "config.h"
#include <v8.h>
#include <slim/slim.h>
#include <slim/command_line_handler.h>
#include <slim/common/log.h>
#include <slim/common/memory_mapper.h>
#include <slim/module/import_specifier.h>
#include <slim/servers/servers.h>
#include <slim/service/launcher.h>
#include <slim/utilities.h>
namespace {
	using namespace slim::common;
	using namespace slim::utilities;
}
void slim::start(int argc, char* argv[]) {
	log::trace(log::Message("slim::start()","begins",__FILE__, __LINE__));
	auto v8_command_line_arguments = slim::command_line::set_argv(argc, argv);
	auto& script_name_string = slim::command_line::get_script_name();
	for(auto&& argument_string : v8_command_line_arguments) {
		log::debug(log::Message("slim::start()","v8 commandline arguments => " + argument_string,__FILE__,__LINE__));	
	}
	log::debug(log::Message("slim::start()","preparing to run script => " + script_name_string,__FILE__,__LINE__));
	slim::service::launcher::marshal_resources(v8_command_line_arguments);
	slim::module::specifier_definition typescript_specifier_definition_struct{"file:///bin/typescript.mjs", memory_mapper::read("typescript_library", "file:///bin/typescript.mjs")};
	auto launch_typescript_future = std::async(std::launch::async, slim::service::launcher::launch, typescript_specifier_definition_struct);
	//slim::servers::start::less();
	//slim::servers::start::prometheus();
	//slim::servers::start::typescript();
			
	if(script_name_string.length() >= 4) {
		log::debug(log::Message("slim::start()","script => " + script_name_string,__FILE__, __LINE__));
		log::debug(log::Message("slim::start()","launching => " + script_name_string,__FILE__, __LINE__));
		auto launch_future = std::async(std::launch::async, slim::service::launcher::launch, script_name_string);
		if(launch_future.valid()) {
			log::debug(log::Message("slim::start()","script future is valid",__FILE__, __LINE__));
			launch_future.get();
			log::debug(log::Message("slim::start()","resolved script future",__FILE__, __LINE__));
		}
		else {
			log::debug(log::Message("slim::start()","future is not valid",__FILE__, __LINE__));
		}
		log::debug(log::Message("slim::start()","called launch",__FILE__, __LINE__));
	}
	log::trace(log::Message("slim::start()","ends",__FILE__, __LINE__));
}
void slim::stop() {
	log::trace(log::Message("slim::stop()","begins",__FILE__, __LINE__));
	slim::service::launcher::tear_down();
	log::trace(log::Message("slim::stop()","ends",__FILE__, __LINE__));
}
void slim::version() {
	log::info("slim:  " VERSION);
	log::info("libv8:  " + std::string(v8::V8::GetVersion()));
}