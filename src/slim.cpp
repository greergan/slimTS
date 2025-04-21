#include <string>
#include <v8.h>
#include "config.h"
#include <slim.h>
#include <slim/builtins.h>
#include <slim/common/exception.h>
#include <slim/common/fetch_and_apply_macros.h>
#include <slim/common/log.h>
#include <slim/gv8.h>
#include <slim/utilities.h>
void slim::run(const std::string file_name, const std::string file_contents) {
	slim::common::log::trace(slim::common::log::Message("slim::run","begins",__FILE__, __LINE__));
	bool is_script_a_module = file_name.ends_with(".mjs") ? true : false;
	auto isolate = slim::gv8::GetIsolate();
	v8::Isolate::Scope isolate_scope(isolate);
	v8::HandleScope handle_scope(isolate);
	slim::common::log::trace(slim::common::log::Message("slim::run","calling slim::gv8::CreateGlobalTemplate()",__FILE__, __LINE__));
	slim::gv8::CreateGlobalTemplate();
	slim::common::log::trace(slim::common::log::Message("slim::run","called slim::gv8::CreateGlobalTemplate()",__FILE__, __LINE__));
	slim::common::log::trace(slim::common::log::Message("slim::run","calling slim::gv8::GetNewContext()",__FILE__, __LINE__));
	auto context = slim::gv8::GetNewContext();
	slim::common::log::trace(slim::common::log::Message("slim::run","called slim::gv8::GetNewContext()",__FILE__, __LINE__));
	if(context.IsEmpty()) {
		slim::common::log::error(slim::common::log::Message("slim::run","Error creating context",__FILE__, __LINE__));
		throw("Error creating context");
	}
	v8::Context::Scope context_scope(context);
	slim::common::log::trace(slim::common::log::Message("slim::run","calling slim::builtins::initialize()",__FILE__, __LINE__));
	slim::builtins::initialize(isolate, slim::gv8::GetGlobalObjectTemplate());
	slim::common::log::trace(slim::common::log::Message("slim::run","called slim::builtins::initialize()",__FILE__, __LINE__));
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
		char* file_name = argv[1];
		std::stringstream script_source_file_contents_stream;
		slim::common::log::trace(slim::common::log::Message("slim::start","calling slim::common::fetch_and_apply_macros()",__FILE__, __LINE__));
		std::string script_source = slim::common::fetch_and_apply_macros(file_name);
		slim::common::log::trace(slim::common::log::Message("slim::start","called slim::common::fetch_and_apply_macros()",__FILE__, __LINE__));
		if(script_source.length() >= 0) {
			slim::common::log::trace(slim::common::log::Message("slim::start","calling slim::gv8::initialize()",__FILE__, __LINE__));
			slim::gv8::initialize(argc, argv);
			slim::common::log::trace(slim::common::log::Message("slim::start","called slim::gv8::initialize()",__FILE__, __LINE__));
			slim::common::log::trace(slim::common::log::Message("slim::start","calling slim::run()",__FILE__, __LINE__));
			slim::run(file_name, script_source);
			slim::common::log::trace(slim::common::log::Message("slim::start","called slim::run()",__FILE__, __LINE__));
			slim::stop();
		}
	}
	slim::common::log::trace(slim::common::log::Message("slim::start","ends",__FILE__, __LINE__));
}
void slim::stop() {
	slim::common::log::trace(slim::common::log::Message("slim::stop","begins",__FILE__, __LINE__));
	slim::gv8::stop();
	slim::common::log::trace(slim::common::log::Message("slim::stop","ends",__FILE__, __LINE__));
}
void slim::version(void) {
/* 	slim::common::log::trace(slim::common::log::Message("slim::version","begins",__FILE__, __LINE__));
    std::cout << "slim:  " << VERSION << "\n";
    std::cout << "libv8: " << std::string(v8::V8::GetVersion()) << "\n"; */
    return;
}