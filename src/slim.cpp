#include <iostream>
#include <fstream>
#include <string>
#include <v8.h>
#include "config.h"
#include <slim.h>
#include <slim/objects.h>
#include <slim/gv8.h>
void slim::run(const std::string file_name, const std::string file_contents) {
	auto isolate = slim::gv8::GetIsolate();
	v8::Isolate::Scope isolate_scope(isolate);
	v8::HandleScope handle_scope(isolate);
	slim::gv8::CreateGlobalTemplate();
	auto context = slim::gv8::GetNewContext();
	if(context.IsEmpty()) {
		throw("Error creating context");
	}
	v8::Context::Scope context_scope(context);
	slim::objects::initialize(isolate);
	v8::TryCatch try_catch(isolate);
	auto script = slim::gv8::CompileScript(file_contents, file_name);
	if(try_catch.HasCaught()) {
		slim::gv8::ReportException(&try_catch);
	}
	if(!script.IsEmpty()) {
		bool result = slim::gv8::RunScript(script);
		if(try_catch.HasCaught()) {
			slim::gv8::ReportException(&try_catch);
		}
	}
	return;
}
void slim::start(int argc, char* argv[]) {
	if(argc == 2) {
		std::string file_contents;
		std::string file_name{argv[1]};
		std::ifstream file(file_name);
		//file.is_open ??
		getline(file, file_contents, '\0');
		file.close();
		if(file_contents.length() > 2) {
			slim::gv8::initialize(argc, argv);
			run(file_name, file_contents);
			stop();
		}
	}
}
void slim::stop() {
	slim::gv8::stop();
}
void slim::version(void) {
    std::cout << "slim:  " << VERSION << "\n";
    std::cout << "libv8: " << std::string(v8::V8::GetVersion()) << "\n";
	std::cout << "asio:  " << SLIM_ASIO_VERSION << "\n";
    return;
}