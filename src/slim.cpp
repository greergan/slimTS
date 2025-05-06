#include <filesystem>
#include <string>
#include <sstream>
#include <v8.h>
#include "config.h"
#include <slim.h>
#include <slim/builtins.h>
#include <slim/command_line_handler.h>
#include <slim/common/exception.h>
#include <slim/common/fetch.h>
#include <slim/common/fetch_and_apply_macros.h>
#include <slim/common/log.h>
#include <slim/gv8.h>
#include <slim/module_resolver.h>
#include <slim/utilities.h>

const std::string get_object_status(v8::Local<v8::Module>& module) {
	std::string status_string;
	switch(module->GetStatus()) {
		case 1:
			status_string = "v8::Module::Status::kUninstantiated";
			break;
		case 2:
			status_string = "v8::Module::Status::kInstantiating";
			break;
		case 3:
			status_string = "v8::Module::Status::kInstantiated";
			break;
		case 4:
			status_string = "v8::Module::Status::kEvaluating";
			break;
		case 5:
			status_string = "v8::Module::Status::kErrored";
			break;
	}
	return status_string;
}
namespace {
	using namespace slim::common::log;
	using namespace slim::utilities;
}
void slim::run(const std::string& file_name, const std::string& file_contents) {
	trace(Message("slim::run","begins",__FILE__, __LINE__));
	bool is_script_a_module = file_name.ends_with(".mjs") ? true : false;
	auto isolate = slim::gv8::GetIsolate();
	v8::Isolate::Scope isolate_scope(isolate);
	v8::HandleScope handle_scope(isolate);
	trace(Message("slim::run","calling slim::gv8::CreateGlobalTemplate()",__FILE__, __LINE__));
	slim::gv8::CreateGlobalTemplate();
	trace(Message("slim::run","called slim::gv8::CreateGlobalTemplate()",__FILE__, __LINE__));
// sort this better please
// not sure why this has to be done before creating a new context
	trace(Message("slim::run","creating builtin stubs",__FILE__, __LINE__));
	auto no_content = [](const v8::FunctionCallbackInfo<v8::Value>& args){};
	slim::gv8::GetGlobalObjectTemplate()->Set(isolate, "setTimeout", v8::FunctionTemplate::New(isolate, no_content));
	slim::gv8::GetGlobalObjectTemplate()->Set(isolate, "clearTimeout", v8::FunctionTemplate::New(isolate, no_content));
	trace(Message("slim::run","created builtin stubs",__FILE__, __LINE__));
	trace(Message("slim::run","calling slim::gv8::GetNewContext()",__FILE__, __LINE__));
	auto context = slim::gv8::GetNewContext();
	trace(Message("slim::run","called slim::gv8::GetNewContext()",__FILE__, __LINE__));
	if(context.IsEmpty()) {
		error(Message("slim::run","Error creating context",__FILE__, __LINE__));
		throw("Error creating context");
	}
	v8::Context::Scope context_scope(context);
	trace(Message("slim::run","creating primordials object",__FILE__, __LINE__));
	context->Global()->Set(context, slim::utilities::StringToV8Name(isolate, "primordials"), v8::Object::New(isolate));
	trace(Message("slim::run","created primordials object",__FILE__, __LINE__));
	trace(Message("slim::run","calling slim::gv8::FetchCompileAndRunJSFunction()",__FILE__, __LINE__));
	std::string primordials_file_name_string("/home/greergan/product/slim/src/plugins/nodejs/lib/internal/per_context/primordials.min.js");
	slim::gv8::FetchCompileAndRunJSFunction(context, primordials_file_name_string);
	trace(Message("slim::run","called slim::gv8::FetchCompileAndRunJSFunction()",__FILE__, __LINE__));
	trace(Message("slim::run","calling slim::builtins::initialize()",__FILE__, __LINE__));
	slim::builtins::initialize(isolate, slim::gv8::GetGlobalObjectTemplate());
	trace(Message("slim::run","called slim::builtins::initialize()",__FILE__, __LINE__));

	v8::TryCatch try_catch(isolate);
	if(true) {
		trace(Message("slim::run","is_script_a_module==true",__FILE__, __LINE__));
		trace(Message("slim::run","calling slim::gv8::CompileAndInstantiateModule",__FILE__, __LINE__));
		auto module = slim::gv8::CompileAndInstantiateModule(file_contents, file_name);
		std::string status_string = get_object_status(module);
		debug(Message("slim::run",(std::string("module->GetStatus() => ") + status_string).c_str(),__FILE__, __LINE__));
		trace(Message("slim::run","called slim::gv8::CompileAndInstantiateModule",__FILE__, __LINE__));
		if(try_catch.HasCaught()) {
			trace(Message("slim::run","try_catch.HasCaught()",__FILE__, __LINE__));
			slim::gv8::ReportException(&try_catch);
		}
		if(!module.IsEmpty()) {
			trace(Message("slim::run","!module.IsEmpty()",__FILE__, __LINE__));
			trace(Message("slim::run","calling module->InstantiateModule()",__FILE__, __LINE__));
			bool module_instantiated = slim::utilities::V8BoolToBool(isolate, module->InstantiateModule(context, slim::gv8::ModuleCallbackResolver));
			std::string status_string = get_object_status(module);
			debug(Message("slim::run",(std::string("module->GetStatus() => ") + status_string).c_str(),__FILE__, __LINE__));
			trace(Message("slim::run","called module->InstantiateModule()",__FILE__, __LINE__));
			if(try_catch.HasCaught()) {
				debug(Message("slim::run","try_catch.HasCaught()",__FILE__, __LINE__));
				slim::gv8::ReportException(&try_catch);
			}
			if(module_instantiated) {
				trace(Message("slim::run","module_instantiated",__FILE__, __LINE__));
				trace(Message("slim::run","calling module->Evaluate()",__FILE__, __LINE__));
				auto result = module->Evaluate(context);
				trace(Message("slim::run","called module->Evaluate()",__FILE__, __LINE__));
				std::string status_string = get_object_status(module);
				trace(Message("slim::run",(std::string("module->GetStatus() => ") + status_string).c_str(),__FILE__, __LINE__));
				if(try_catch.HasCaught()) {
					trace(Message("slim::run","try_catch.HasCaught()",__FILE__, __LINE__));
					slim::gv8::ReportException(&try_catch);
				}
				if(module->GetStatus() == v8::Module::Status::kErrored) {
					std::string error = slim::utilities::v8ValueToString(isolate, module->GetException());
					slim::common::log::error(Message("slim::run",error.c_str(),__FILE__, __LINE__));
					isolate->ThrowException(module->GetException());
				}
				if(try_catch.HasCaught()) {
					trace(Message("slim::run","try_catch.HasCaught()",__FILE__, __LINE__));
					slim::gv8::ReportException(&try_catch);
				}
			}
			else {
				trace(Message("slim::run","!module_instantiated",__FILE__, __LINE__));
				throw("Initial module instantiation failed");
			}
		}
	}
	else {
		trace(Message("slim::run","is a script",__FILE__, __LINE__));
		trace(Message("slim::run","calling slim::gv8::CompileScript()",__FILE__, __LINE__));
		auto script = slim::gv8::CompileScript(file_contents, file_name);
		trace(Message("slim::run","called slim::gv8::CompileScript()",__FILE__, __LINE__));
		if(try_catch.HasCaught()) {
			trace(Message("slim::run","try_catch.HasCaught()",__FILE__, __LINE__));
			slim::gv8::ReportException(&try_catch);
		}
		if(!script.IsEmpty()) {
			trace(Message("slim::run","calling slim::gv8::RunScript()",__FILE__, __LINE__));
			bool result = slim::gv8::RunScript(script);
			trace(Message("slim::run","called slim::gv8::RunScript()",__FILE__, __LINE__));
			if(try_catch.HasCaught()) {
				trace(Message("slim::run","try_catch.HasCaught()",__FILE__, __LINE__));
				slim::gv8::ReportException(&try_catch);
			}
		}
	}
	trace(Message("slim::run","ends",__FILE__, __LINE__));
	return;
}
void slim::start(int argc, char* argv[]) {
	trace(Message("slim::start()","begins",__FILE__, __LINE__));
	slim::command_line::set_argv(argc, argv);
	trace(Message("slim::start()",std::string("command line arguments => " + std::to_string(argc)).c_str(),__FILE__, __LINE__));
	if(argc > 1) {
		std::string file_name_string(argv[1]);
		std::stringstream script_source_file_contents_stream;
		trace(Message("slim::start()",std::string("calling slim::common::fetch_and_apply_macros() for file =>" + file_name_string).c_str(),__FILE__, __LINE__));
		std::string script_source = slim::common::fetch_and_apply_macros(file_name_string);
		trace(Message("slim::start()","called slim::common::fetch_and_apply_macros()",__FILE__, __LINE__));
		if(script_source.length() >= 0) {
			trace(Message("slim::start()","calling slim::gv8::initialize()",__FILE__, __LINE__));
			slim::gv8::initialize(argc, argv);
			trace(Message("slim::start()","called slim::gv8::initialize()",__FILE__, __LINE__));
			trace(Message("slim::start()","calling slim::run()",__FILE__, __LINE__));
			slim::run(file_name_string, script_source);
			trace(Message("slim::start()","called slim::run()",__FILE__, __LINE__));
			slim::stop();
		}
	}
	trace(Message("slim::start()","ends",__FILE__, __LINE__));
}
void slim::stop() {
	trace(Message("slim::stop","begins",__FILE__, __LINE__));
	slim::gv8::stop();
	trace(Message("slim::stop","ends",__FILE__, __LINE__));
}
void slim::version(void) {
/* 	trace(Message("slim::version","begins",__FILE__, __LINE__));
    std::cout << "slim:  " << VERSION << "\n";
    std::cout << "libv8: " << std::string(v8::V8::GetVersion()) << "\n"; */
    return;
}