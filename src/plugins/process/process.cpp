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
	std::string TSC_WATCHFILE; // typescript env variable
	v8::Local<v8::Array> args_array;
	void exit_wrapper(const v8::FunctionCallbackInfo<v8::Value>& args);
	void nextTick(const v8::FunctionCallbackInfo<v8::Value>& args);
	void platform(const v8::FunctionCallbackInfo<v8::Value>& args);
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
extern "C" void expose_plugin(v8::Isolate* isolate) {
	slim::plugin::plugin process_plugin(isolate, "process");
	slim::plugin::plugin process_env_plugin(isolate, "env");
	process_env_plugin.add_property("TSC_WATCHFILE", &slim::plugin::process::TSC_WATCHFILE);
	process_plugin.add_property("browser", &slim::plugin::process::browser);
	process_plugin.add_function("exit", slim::plugin::process::exit_wrapper);
	process_plugin.add_function("nextTick", slim::plugin::process::nextTick);
	process_plugin.add_function("platform", slim::plugin::process::platform);
	process_plugin.add_plugin("env", &process_env_plugin);
	v8::Local<v8::Context> context = isolate->GetCurrentContext();
	v8::Local<v8::Object> full_process_plugin_object = process_plugin.new_instance();
	v8::Local<v8::Array> argv_array = v8::Array::New(isolate);
	std::vector<std::string> argv_vector = slim::command_line::get_argv();
	uint32_t argv_index = 0;
	for(auto& command_line_argument: argv_vector) {
		argv_array->Set(context, argv_index, slim::utilities::StringToV8Value(isolate, command_line_argument));
		argv_index++;
	}
	v8::Maybe<bool> set_results = full_process_plugin_object->Set(isolate->GetCurrentContext(), slim::utilities::StringToV8String(isolate, "argv"), argv_array);
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