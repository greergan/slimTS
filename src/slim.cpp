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
namespace {
	using namespace slim::common::log;
	using namespace slim::utilities;
}
void slim::run(std::string file_name_string_in) {
	trace(Message("slim::run()","begins",__FILE__, __LINE__));
	bool is_script_a_module = true; //file_name.ends_with(".mjs") ? true : false;
	auto isolate = slim::gv8::GetIsolate();
	v8::Isolate::Scope isolate_scope(isolate);
	v8::HandleScope handle_scope(isolate);
	debug(Message("slim::run","calling slim::gv8::CreateGlobalTemplate()",__FILE__, __LINE__));
	slim::gv8::CreateGlobalTemplate();
	debug(Message("slim::run","called slim::gv8::CreateGlobalTemplate()",__FILE__, __LINE__));
// sort this better please
// not sure why this has to be done before creating a new context
debug(Message("slim::run","creating builtin stubs",__FILE__, __LINE__));
	auto no_content = [](const v8::FunctionCallbackInfo<v8::Value>& args){};
	slim::gv8::GetGlobalObjectTemplate()->Set(isolate, "setTimeout", v8::FunctionTemplate::New(isolate, no_content));
	slim::gv8::GetGlobalObjectTemplate()->Set(isolate, "clearTimeout", v8::FunctionTemplate::New(isolate, no_content));
	debug(Message("slim::run","created builtin stubs",__FILE__, __LINE__));
	debug(Message("slim::run","calling slim::gv8::GetNewContext()",__FILE__, __LINE__));
	auto context = slim::gv8::GetNewContext();
	debug(Message("slim::run","called slim::gv8::GetNewContext()",__FILE__, __LINE__));
	if(context.IsEmpty()) {
		error(Message("slim::run","Error creating context",__FILE__, __LINE__));
		throw("Error creating context");
	}
	v8::Context::Scope context_scope(context);
	debug(Message("slim::run","creating primordials object",__FILE__, __LINE__));
	context->Global()->Set(context, slim::utilities::StringToV8Name(isolate, "primordials"), v8::Object::New(isolate));
	debug(Message("slim::run","created primordials object",__FILE__, __LINE__));
	debug(Message("slim::run","calling slim::gv8::FetchCompileAndRunJSFunction()",__FILE__, __LINE__));
	std::string primordials_file_name_string("/home/greergan/product/slim/src/plugins/nodejs/lib/internal/per_context/primordials.min.js");
	slim::gv8::FetchCompileAndRunJSFunction(context, primordials_file_name_string);
	debug(Message("slim::run","called slim::gv8::FetchCompileAndRunJSFunction()",__FILE__, __LINE__));
	debug(Message("slim::run","calling slim::builtins::initialize()",__FILE__, __LINE__));
	slim::builtins::initialize(isolate, slim::gv8::GetGlobalObjectTemplate());
	debug(Message("slim::run","called slim::builtins::initialize()",__FILE__, __LINE__));
	{
		v8::TryCatch try_catch(isolate);
		debug(Message("slim::run()", "calling  resolve_imports()",__FILE__, __LINE__));
		bool is_entry_point = true;
		auto& import_module_cache = slim::module::resolver::resolve_imports(file_name_string_in, context, is_entry_point);
		for(auto& [name, obj] : import_module_cache) {
			debug(Message("module name", std::string(name).c_str(),__FILE__,__LINE__));
		}
		debug(Message("slim::run()",std::string("found this many modules => " + std::to_string(import_module_cache.size())).c_str(),__FILE__, __LINE__));
		std::string entry_point_specifier_string;
		for(auto& [specifier_string, import_specifier_struct] : import_module_cache) {
			if(import_specifier_struct.is_entry_point()) {
				entry_point_specifier_string = specifier_string;
			}
		}
		if(entry_point_specifier_string.size() > 0 && import_module_cache[entry_point_specifier_string].has_module()) {
			auto& module = import_module_cache[entry_point_specifier_string].get_module();
			if(module->GetStatus() == v8::Module::Status::kErrored) {
				isolate->ThrowException(module->GetException());
			}
			else {
				auto result = module->Evaluate(context);
			}
		}
		if(try_catch.HasCaught()) {
			error(Message("slim::run","try_catch.HasCaught()",__FILE__, __LINE__));
			slim::gv8::ReportException(&try_catch);
		}
	}
/* 	else {
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
	} */
	trace(Message("slim::run()","ends",__FILE__, __LINE__));
	return;
}
void slim::start(int argc, char* argv[]) {
	trace(Message("slim::start()","begins",__FILE__, __LINE__));
	slim::command_line::set_argv(argc, argv);
	info(Message("slim::start()",std::string("command line arguments => " + std::to_string(argc)).c_str(),__FILE__, __LINE__));
	if(argc > 1) {
		const std::string file_name_string(argv[1]);
		debug(Message("slim::start()","calling slim::gv8::initialize()",__FILE__, __LINE__));
		slim::gv8::initialize(argc, argv);
		debug(Message("slim::start()","called slim::gv8::initialize()",__FILE__, __LINE__));
		debug(Message("slim::start()","calling slim::run()",__FILE__, __LINE__));
		slim::run(file_name_string);
		debug(Message("slim::start()","called slim::run()",__FILE__, __LINE__));
		slim::stop();
	}
	trace(Message("slim::start()","ends",__FILE__, __LINE__));
}
void slim::stop() {
	trace(Message("slim::stop()","begins",__FILE__, __LINE__));
	slim::gv8::stop();
	trace(Message("slim::stop()","ends",__FILE__, __LINE__));
}
void slim::version(void) {
/* 	trace(Message("slim::version","begins",__FILE__, __LINE__));
    std::cout << "slim:  " << VERSION << "\n";
    std::cout << "libv8: " << std::string(v8::V8::GetVersion()) << "\n"; */
    return;
}