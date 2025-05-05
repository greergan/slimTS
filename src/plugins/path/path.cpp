#include <v8.h>
#include <slim/gv8.h>
#include <slim/plugin.hpp>
#include <slim/utilities.h>
namespace slim::plugin::path {
	//void platform(const v8::FunctionCallbackInfo<v8::Value>& args);
}
/* void slim::plugin::path::realpathSync::native(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	args.GetReturnValue().Set(slim::utilities::StringToV8String(isolate, "linux"));
} */
extern "C" void expose_plugin(v8::Isolate* isolate) {
	slim::plugin::plugin path_plugin(isolate, "path");
	path_plugin.expose_plugin();
	return;
}

/*

process !== "undefined" && !!process.nextTick && !process.browser

https://nodejs.org/en/learn/asynchronous-work/understanding-processnexttick

*/