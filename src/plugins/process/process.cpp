#include <sys/types.h>
#include <unistd.h>
#include <filesystem>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <v8.h>
#include <slim/command_line_handler.h>
#include <slim/common/log.h>
#include <slim/common/platform.h>
#include <slim/common/memory_mapper.h>
#include <slim/plugin.hpp>
#include <slim/utilities.h>
namespace slim::plugin::process {
	std::filesystem::path current_working_directory;
	std::string TSC_WATCHFILE; // typescript env variable
	v8::Local<v8::Array> args_array;
	void cwd(const v8::FunctionCallbackInfo<v8::Value>& args);
	void exit_wrapper(const v8::FunctionCallbackInfo<v8::Value>& args);
	void nextTick(const v8::FunctionCallbackInfo<v8::Value>& args);
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
void slim::plugin::process::stderr_write(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	std::string value = slim::utilities::v8ValueToString(isolate, args[0]);
	std::cerr << value;
}
void slim::plugin::process::stdout_write(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	std::string value = slim::utilities::v8ValueToString(isolate, args[0]);
	std::cout << value;
}
extern "C" void expose_plugin(v8::Isolate* isolate) {
	v8::HandleScope isolate_scope(isolate);
	auto context = isolate->GetCurrentContext();
	slim::plugin::plugin process_plugin(isolate, "process");
	slim::plugin::plugin process_env_plugin(isolate, "env");
	slim::plugin::plugin process_stderr_plugin(isolate, "stderr");
	slim::plugin::plugin process_stdout_plugin(isolate, "stdout");
	process_plugin.add_property_immutable("pid", getpid());
	process_plugin.add_property_immutable("platform", slim::common::platform::platform);
	process_env_plugin.add_property("TSC_WATCHFILE", &slim::plugin::process::TSC_WATCHFILE);
	process_plugin.add_function("cwd", 	slim::plugin::process::cwd);
	process_plugin.add_function("exit", slim::plugin::process::exit_wrapper);
	process_plugin.add_function("nextTick", slim::plugin::process::nextTick);
	process_stderr_plugin.add_function("write", slim::plugin::process::stderr_write);
	process_stdout_plugin.add_function("write", slim::plugin::process::stdout_write);
	process_plugin.add_plugin("env", &process_env_plugin);
	process_plugin.add_plugin("stderr", &process_stderr_plugin);
	process_plugin.add_plugin("stdout", &process_stdout_plugin);
	auto v8_argv_array = v8::Array::New(isolate);
	std::stringstream script_argv_stream(*slim::common::memory_mapper::read("configurations", "script.argv").get());
	std::string arg;
	int argc = 0;
	while(std::getline(script_argv_stream, arg, ',')) {
		auto result = v8_argv_array->Set(context, argc, slim::utilities::StringToV8Value(isolate, arg));
		argc++;
	}
	auto instance_of_object = process_plugin.new_instance();
	instance_of_object->Set(context, slim::utilities::StringToV8String(isolate, "argv"), v8_argv_array);
	process_plugin.expose_plugin(instance_of_object);
	return;
}
