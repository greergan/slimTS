#include <filesystem>
#include <future>
#include <string>
#include "config.h"
#include <v8.h>
#include <slim.h>
#include <slim/command_line_handler.h>
#include <slim/common/log.h>
#include <slim/launcher.h>
#include <slim/metrics_server.h>
#include <slim/utilities.h>
namespace {
	using namespace slim::common;
	using namespace slim::utilities;
}
void slim::start(int argc, char* argv[]) {
	log::trace(log::Message("slim::start()","begins",__FILE__, __LINE__));
	auto v8_command_line_arguments = slim::command_line::set_argv(argc, argv);
	auto& script_name = slim::command_line::get_script_name();
	slim::metrics::server::start();
	if(script_name.length() >= 4) {
		log::debug(log::Message("slim::start()",std::string("script => " + script_name).c_str(),__FILE__, __LINE__));
		if(v8_command_line_arguments.size() > 0) {
			log::debug(log::Message("slim::start()","some v8 command line arguments",__FILE__, __LINE__));
		}
		else {
			log::debug(log::Message("slim::start()","without any v8 command line arguments",__FILE__, __LINE__));
		}
		slim::launcher::initialize(v8_command_line_arguments);
		log::debug(log::Message("slim::start()","calling launch",__FILE__, __LINE__));
		auto launch_future = std::async(std::launch::async, slim::launcher::launch, script_name);
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
	slim::launcher::tear_down();
	log::trace(log::Message("slim::stop()","ends",__FILE__, __LINE__));
}
void slim::version() {
	log::info("slim:  " VERSION);
	log::info("libv8:  " + std::string(v8::V8::GetVersion()));
}