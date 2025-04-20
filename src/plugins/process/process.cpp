#include <v8.h>
#include <slim/gv8.h>
#include <slim/plugin.hpp>
#include <slim/utilities.h>
namespace slim::plugin::process {
	bool browser;
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
	process_plugin.add_property("browser", &slim::plugin::process::browser);
	process_plugin.add_function("exit", slim::plugin::process::exit_wrapper);
	process_plugin.add_function("nextTick", slim::plugin::process::nextTick);
	process_plugin.add_function("platform", slim::plugin::process::platform);
	process_plugin.expose_plugin();
	return;
}

/*

process !== "undefined" && !!process.nextTick && !process.browser

https://nodejs.org/en/learn/asynchronous-work/understanding-processnexttick

*/