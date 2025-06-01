#include <filesystem>
#include <string>
#include <sstream>
#include <v8.h>
#include "config.h"
#include <slim.h>
#include <slim/builtins.h>
#include <slim/builtins/typescript.h>
#include <slim/command_line_handler.h>
#include <slim/common/exception.h>
#include <slim/common/log.h>
#include <slim/gv8.h>
#include <slim/module_resolver.h>
#include <slim/utilities.h>

#include <iostream>
namespace {
	using namespace slim::common;
	using namespace slim::utilities;
	using namespace v8;
	using namespace std;
	void PrintStackTrace(Isolate* isolate, Local<Value> error) {
		if(error->IsObject()) {
			Local<Object> error_obj = error->ToObject(isolate->GetCurrentContext()).ToLocalChecked();
			Local<String> message_key = String::NewFromUtf8Literal(isolate, "message");
			Local<Value> message_value;
			Local<String> stack_key = String::NewFromUtf8Literal(isolate, "stack");
			Local<Value> stack_value;
			if(error_obj->Get(isolate->GetCurrentContext(), StringToV8String(isolate, "stack")).ToLocal(&stack_value) && stack_value->IsString()) {
				cout << "\n" << v8ValueToString(isolate, stack_value) << "\n";
			}
			return;
		}
    	cout << "\n" << v8ValueToString(isolate, error) << "\n";
	}
	void OnPromiseRejected(PromiseRejectMessage message) {
		Isolate* isolate = message.GetPromise()->GetIsolate();
		Local<Value> value = message.GetValue();
		PrintStackTrace(isolate, value);
	}
}
void slim::run(std::string file_name_string_in) {
	log::trace(log::Message("slim::run()","begins",__FILE__, __LINE__));
	bool is_script_a_module = file_name_string_in.ends_with(".mjs") ? true : false;
	bool is_typescript_module = file_name_string_in.ends_with(".ts") ? true : false;
	if(is_typescript_module) {
		is_script_a_module = true;
	}
	auto isolate = slim::gv8::GetIsolate();
	isolate->SetPromiseRejectCallback(OnPromiseRejected);
	v8::Isolate::Scope isolate_scope(isolate);
	v8::HandleScope handle_scope(isolate);
	log::debug(log::Message("slim::run","calling slim::gv8::CreateGlobalTemplate()",__FILE__, __LINE__));
	slim::gv8::CreateGlobalTemplate();
	log::debug(log::Message("slim::run","called slim::gv8::CreateGlobalTemplate()",__FILE__, __LINE__));
// sort this better please
// not sure why this has to be done before creating a new context
log::debug(log::Message("slim::run","creating builtin stubs",__FILE__, __LINE__));
	auto no_content = [](const v8::FunctionCallbackInfo<v8::Value>& args){};
	slim::gv8::GetGlobalObjectTemplate()->Set(isolate, "setTimeout", v8::FunctionTemplate::New(isolate, no_content));
	slim::gv8::GetGlobalObjectTemplate()->Set(isolate, "clearTimeout", v8::FunctionTemplate::New(isolate, no_content));
	slim::gv8::GetGlobalObjectTemplate()->Set(isolate, "require", v8::FunctionTemplate::New(isolate, slim::builtins::require));
	log::debug(log::Message("slim::run","created builtin stubs",__FILE__, __LINE__));
	log::debug(log::Message("slim::run","calling slim::gv8::GetNewContext()",__FILE__, __LINE__));
	auto context = slim::gv8::GetNewContext();
	log::debug(log::Message("slim::run","called slim::gv8::GetNewContext()",__FILE__, __LINE__));
	if(context.IsEmpty()) {
		log::error(log::Message("slim::run","Error creating context",__FILE__, __LINE__));
		throw("Error creating context");
	}
	v8::Context::Scope context_scope(context);
	log::debug(log::Message("slim::run","calling slim::builtins::initialize()",__FILE__, __LINE__));
	slim::builtins::initialize(isolate, slim::gv8::GetGlobalObjectTemplate());
	log::debug(log::Message("slim::run","called slim::builtins::initialize()",__FILE__, __LINE__));
	v8::TryCatch try_catch(isolate);
	log::debug(log::Message("slim::run()", "calling  resolve_imports()",__FILE__, __LINE__));
	bool is_entry_point = true;
	auto module_import_specifier_ptr = slim::module::resolver::resolve_imports(file_name_string_in, context, is_entry_point);
	log::debug(log::Message("slim::run()", std::string("get_module_status_string() => " + module_import_specifier_ptr->get_module_status_string()).c_str(),__FILE__, __LINE__));
	if(module_import_specifier_ptr->get_module()->GetStatus() == v8::Module::Status::kErrored) {
		isolate->ThrowException(module_import_specifier_ptr->get_module()->GetException());
	}
	else {
		auto result = module_import_specifier_ptr->get_module()->Evaluate(context);
		if(module_import_specifier_ptr->get_module()->GetStatus() == v8::Module::Status::kErrored) {
			isolate->ThrowException(module_import_specifier_ptr->get_module()->GetException());
		}
		log::debug(log::Message("slim::run()", std::string("get_module_status_string() => " + module_import_specifier_ptr->get_module_status_string()).c_str(),__FILE__, __LINE__));
	}
	if(try_catch.HasCaught()) {
		log::error(log::Message("slim::run","try_catch.HasCaught()",__FILE__, __LINE__));
		slim::gv8::ReportException(&try_catch);
	}
	log::trace(log::Message("slim::run()","ends",__FILE__, __LINE__));
	return;
}
void slim::start(int argc, char* argv[]) {
	log::trace(log::Message("slim::start()","begins",__FILE__, __LINE__));
	auto v8_command_line_arguments = slim::command_line::set_argv(argc, argv);
	auto& script_name = slim::command_line::get_script_name();
	if(script_name.length() >= 4) {
		log::debug(log::Message("slim::start()",std::string("script => " + script_name).c_str(),__FILE__, __LINE__));
		if(v8_command_line_arguments) {
			log::debug(log::Message("slim::start()","some v8 command line arguments",__FILE__, __LINE__));
			slim::gv8::initialize(sizeof(v8_command_line_arguments) / sizeof(v8_command_line_arguments[0]), v8_command_line_arguments);
		}
		else {
			log::debug(log::Message("slim::start()","without any v8 command line arguments",__FILE__, __LINE__));
			char* empty[0];
			slim::gv8::initialize(0, empty);
		}
		slim::run(script_name);
		slim::stop();
	}
	log::trace(log::Message("slim::start()","ends",__FILE__, __LINE__));
}
void slim::stop() {
	log::trace(log::Message("slim::stop()","begins",__FILE__, __LINE__));
	slim::gv8::stop();
	log::trace(log::Message("slim::stop()","ends",__FILE__, __LINE__));
}
void slim::version(void) {
 	//trace(Message("slim::version","begins",__FILE__, __LINE__));
    std::cout << "slim:  " << VERSION << "\n";
    std::cout << "libv8: " << std::string(v8::V8::GetVersion()) << "\n";
    return;
}