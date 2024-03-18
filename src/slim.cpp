#include <iostream>
#include <fstream>
#include <string>
#include <v8.h>
#include "config.h"
#include <slim.h>
#include <slim/builtins.h>
#include <slim/objects.h>
#include <slim/gv8.h>
#include <slim/utilities.h>

void slim::run(const std::string file_name, const std::string file_contents) {
	bool is_module = file_name.ends_with(".mjs") ? true : false;
	auto isolate = slim::gv8::GetIsolate();
	//v8::Isolate::Scope isolate_scope(isolate);
	v8::HandleScope handle_scope(isolate);
	slim::gv8::CreateGlobalTemplate();
	//must load plugins to be used on the global isolate prior to context scoping
	slim::builtins::initialize(isolate, slim::gv8::GetGlobalObjectTemplate());
	auto context = slim::gv8::GetNewContext();
	if(context.IsEmpty()) {
		throw("Error creating context");
	}
	v8::Context::Scope context_scope(context);
	slim::objects::initialize(isolate);
	v8::TryCatch try_catch(isolate);
	if(is_module) {
		auto module = slim::gv8::CompileAndInstantiateModule(file_contents, file_name);
		if(try_catch.HasCaught()) {
			slim::gv8::ReportException(&try_catch);
		}
		if(!module.IsEmpty()) {
			bool instantiated = slim::utilities::V8BoolToBool(isolate, module->InstantiateModule(context, slim::gv8::ModuleCallbackResolver));
			if(try_catch.HasCaught()) {
				slim::gv8::ReportException(&try_catch);
			}
			if(instantiated) {
				auto result = module->Evaluate(context);
				if(try_catch.HasCaught()) {
					slim::gv8::ReportException(&try_catch);
				}
			}
			else {
				std::cout << "Initial module instantiation failed\n";
			}
		}
	}
	else {
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
    return;
}