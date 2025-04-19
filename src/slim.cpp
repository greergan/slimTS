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
#include <slim/common/log.h>
void slim::run(const std::string file_name, const std::string file_contents) {
	slim::common::log::trace(slim::common::log::Message("slim::run","begins",__FILE__, __LINE__));
	bool is_script_a_module = file_name.ends_with(".mjs") ? true : false;
	auto isolate = slim::gv8::GetIsolate();
	v8::Isolate::Scope isolate_scope(isolate);
	v8::HandleScope handle_scope(isolate);
	slim::gv8::CreateGlobalTemplate();
	//must load plugins to be used on the global isolate prior to context scoping
	slim::builtins::initialize(isolate, slim::gv8::GetGlobalObjectTemplate());
	auto context = slim::gv8::GetNewContext();
	if(context.IsEmpty()) {
		slim::common::log::error(slim::common::log::Message("slim::run","Error creating context",__FILE__, __LINE__));
		throw("Error creating context");
	}
	v8::Context::Scope context_scope(context);
	slim::objects::initialize(isolate);
	v8::TryCatch try_catch(isolate);
	if(is_script_a_module) {
		slim::common::log::trace(slim::common::log::Message("slim::run","is_script_a_module==true",__FILE__, __LINE__));
		slim::common::log::trace(slim::common::log::Message("slim::run","calling slim::gv8::CompileAndInstantiateModule",__FILE__, __LINE__));
		auto module = slim::gv8::CompileAndInstantiateModule(file_contents, file_name);
		slim::common::log::trace(slim::common::log::Message("slim::run","called slim::gv8::CompileAndInstantiateModule",__FILE__, __LINE__));
		if(try_catch.HasCaught()) {
			slim::common::log::trace(slim::common::log::Message("slim::run","try_catch.HasCaught()",__FILE__, __LINE__));
			slim::gv8::ReportException(&try_catch);
		}
		if(!module.IsEmpty()) {
			slim::common::log::trace(slim::common::log::Message("slim::run","!module.IsEmpty()",__FILE__, __LINE__));
			slim::common::log::trace(slim::common::log::Message("slim::run","calling module->InstantiateModule()",__FILE__, __LINE__));
			bool module_instantiated = slim::utilities::V8BoolToBool(isolate, module->InstantiateModule(context, slim::gv8::ModuleCallbackResolver));
			slim::common::log::trace(slim::common::log::Message("slim::run","called module->InstantiateModule()",__FILE__, __LINE__));
			if(try_catch.HasCaught()) {
				slim::common::log::trace(slim::common::log::Message("slim::run","try_catch.HasCaught()",__FILE__, __LINE__));
				slim::gv8::ReportException(&try_catch);
			}
			if(module_instantiated) {
				slim::common::log::trace(slim::common::log::Message("slim::run","module_instantiated",__FILE__, __LINE__));
				slim::common::log::trace(slim::common::log::Message("slim::run","calling module->Evaluate()",__FILE__, __LINE__));
				auto result = module->Evaluate(context);
				slim::common::log::trace(slim::common::log::Message("slim::run","called module->Evaluate()",__FILE__, __LINE__));
				slim::common::log::trace(slim::common::log::Message("slim::run",(std::string("module->GetStatus() => ") + std::to_string(module->GetStatus())).c_str(),__FILE__, __LINE__));
				if(module->GetStatus() == 5) {
					slim::common::log::error(slim::common::log::Message("slim::run",slim::utilities::v8ValueToString(isolate, module->GetException()).c_str(),__FILE__, __LINE__));
					slim::common::log::error(slim::common::log::Message("slim::run","calling isolate->ThrowException(module->GetException())",__FILE__, __LINE__));
					isolate->ThrowException(module->GetException());
				}
				if(try_catch.HasCaught()) {
					slim::common::log::trace(slim::common::log::Message("slim::run","try_catch.HasCaught()",__FILE__, __LINE__));
					slim::gv8::ReportException(&try_catch);
				}
			}
			else {
				slim::common::log::trace(slim::common::log::Message("slim::run","!module_instantiated",__FILE__, __LINE__));
				throw("Initial module instantiation failed");
			}
		}
	}
	else {
		slim::common::log::trace(slim::common::log::Message("slim::run","is a script",__FILE__, __LINE__));
		slim::common::log::trace(slim::common::log::Message("slim::run","calling slim::gv8::CompileScript()",__FILE__, __LINE__));
		auto script = slim::gv8::CompileScript(file_contents, file_name);
		slim::common::log::trace(slim::common::log::Message("slim::run","called slim::gv8::CompileScript()",__FILE__, __LINE__));
		if(try_catch.HasCaught()) {
			slim::common::log::trace(slim::common::log::Message("slim::run","try_catch.HasCaught()",__FILE__, __LINE__));
			slim::gv8::ReportException(&try_catch);
		}
		if(!script.IsEmpty()) {
			slim::common::log::trace(slim::common::log::Message("slim::run","calling slim::gv8::RunScript()",__FILE__, __LINE__));
			bool result = slim::gv8::RunScript(script);
			slim::common::log::trace(slim::common::log::Message("slim::run","called slim::gv8::RunScript()",__FILE__, __LINE__));
			if(try_catch.HasCaught()) {
				slim::common::log::trace(slim::common::log::Message("slim::run","try_catch.HasCaught()",__FILE__, __LINE__));
				slim::gv8::ReportException(&try_catch);
			}
		}
	}
	slim::common::log::trace(slim::common::log::Message("slim::run","ends",__FILE__, __LINE__));
	return;
}
void slim::start(int argc, char* argv[]) {
	slim::common::log::trace(slim::common::log::Message("slim::start","begins",__FILE__, __LINE__));
	slim::common::log::trace(slim::common::log::Message("slim::start",std::string("command line arguments => " + std::to_string(argc)).c_str(),__FILE__, __LINE__));
	if(argc > 1) {
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
	slim::common::log::trace(slim::common::log::Message("slim::start","ends",__FILE__, __LINE__));
}
void slim::stop() {
	slim::gv8::stop();
}
void slim::version(void) {
    std::cout << "slim:  " << VERSION << "\n";
    std::cout << "libv8: " << std::string(v8::V8::GetVersion()) << "\n";
    return;
}