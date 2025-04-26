#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <v8.h>
#include <slim/command_line_handler.h>
#include <slim/common/log.h>
#include <slim/gv8.h>
#include <slim/plugin.hpp>
#include <slim/utilities.h>
namespace slim::plugin::process {
	bool browser;
	std::filesystem::path current_working_directory;
	std::string TSC_WATCHFILE; // typescript env variable
	v8::Local<v8::Array> args_array;
	void cwd(const v8::FunctionCallbackInfo<v8::Value>& args);
	void exit_wrapper(const v8::FunctionCallbackInfo<v8::Value>& args);
	void nextTick(const v8::FunctionCallbackInfo<v8::Value>& args);
	void platform(const v8::FunctionCallbackInfo<v8::Value>& args);
	void stderr_write(const v8::FunctionCallbackInfo<v8::Value>& args);
	void stdout_write(const v8::FunctionCallbackInfo<v8::Value>& args);
}
void slim::plugin::process::cwd(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if(slim::plugin::process::current_working_directory.empty()) {
		slim::plugin::process::current_working_directory = std::filesystem::current_path();
	}
	args.GetReturnValue().Set(slim::utilities::StringToV8String(isolate, slim::plugin::process::current_working_directory));
}
void slim::plugin::process::exit_wrapper(const v8::FunctionCallbackInfo<v8::Value>& args) {
	exit(1);
}
void slim::plugin::process::nextTick(const v8::FunctionCallbackInfo<v8::Value>& args) {

}
void slim::plugin::process::platform(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	args.GetReturnValue().Set(slim::utilities::StringToV8String(isolate, "linux"));
}
void slim::plugin::process::stderr_write(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	std::string value = slim::utilities::v8ValueToString(isolate, args[0]);
	std::cerr << value << std::endl;
}
void slim::plugin::process::stdout_write(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	std::string value = slim::utilities::v8ValueToString(isolate, args[0]);
	std::cout << value << std::endl;
}
extern "C" void expose_plugin(v8::Isolate* isolate) {
	slim::plugin::plugin process_plugin(isolate, "process");
	slim::plugin::plugin process_env_plugin(isolate, "env");
	slim::plugin::plugin process_stderr_plugin(isolate, "stderr");
	slim::plugin::plugin process_stdout_plugin(isolate, "stdout");
	process_env_plugin.add_property("TSC_WATCHFILE", &slim::plugin::process::TSC_WATCHFILE);
	process_plugin.add_property("browser", &slim::plugin::process::browser);
	process_plugin.add_function("cwd", 	slim::plugin::process::cwd);
	process_plugin.add_function("exit", slim::plugin::process::exit_wrapper);
	process_plugin.add_function("nextTick", slim::plugin::process::nextTick);
	process_plugin.add_function("platform", slim::plugin::process::platform);
	process_stderr_plugin.add_function("write", slim::plugin::process::stderr_write);
	process_stdout_plugin.add_function("write", slim::plugin::process::stdout_write);
	process_plugin.add_plugin("env", &process_env_plugin);
	process_plugin.add_plugin("stderr", &process_stderr_plugin);
	process_plugin.add_plugin("stdout", &process_stdout_plugin);
	v8::Local<v8::Context> context = isolate->GetCurrentContext();
	v8::Local<v8::Object> full_process_plugin_object = process_plugin.new_instance();
	v8::Local<v8::Array> argv_array = v8::Array::New(isolate);
	std::vector<std::string> argv_vector = slim::command_line::get_argv();
	uint32_t argv_index = 0;
	for(auto& command_line_argument: argv_vector) {
		auto result = argv_array->Set(context, argv_index, slim::utilities::StringToV8Value(isolate, command_line_argument));
		argv_index++;
	}
	v8::Maybe<bool> set_results = full_process_plugin_object->Set(context, slim::utilities::StringToV8String(isolate, "argv"), argv_array);
	if(!set_results.IsJust()) {
		isolate->ThrowException(slim::utilities::StringToV8String(isolate, "slim::plugin::process::expose_plugin() failed to load object properties"));
	}
	process_plugin.expose_plugin(full_process_plugin_object);
	return;
}

/*

process !== "undefined" && !!process.nextTick && !process.browser

https://nodejs.org/en/learn/asynchronous-work/understanding-processnexttick

*/