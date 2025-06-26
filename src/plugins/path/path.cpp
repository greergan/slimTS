#include <v8.h>
#include <slim/common/log.h>
#include <slim/plugin.hpp>
#include <slim/utilities.h>
namespace slim::plugin::path {
	using namespace slim::common;
	using namespace slim::utilities;
	void join(const v8::FunctionCallbackInfo<v8::Value>& args);
}
/* void slim::plugin::path::realpathSync::native(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	args.GetReturnValue().Set(slim::utilities::StringToV8String(isolate, "linux"));
} */
void slim::plugin::path::join(const v8::FunctionCallbackInfo<v8::Value>& args) {
	log::trace(log::Message("slim::plugin::path::join()", "begins",__FILE__, __LINE__));
	auto* isolate = args.GetIsolate();
	std::string joined_strings;
	for(int index = 0; index < args.Length(); index++) {
		if(joined_strings.length() > 0 && !joined_strings.ends_with('/')) {
			joined_strings += "/";
		}
		joined_strings += utilities::v8ValueToString(isolate, args[index]);
	}
	log::debug(log::Message("slim::plugin::path::join() => ", joined_strings.c_str(), __FILE__, __LINE__));
	args.GetReturnValue().Set(utilities::StringToV8String(isolate, joined_strings));
	log::trace(log::Message("slim::plugin::path::join()", "ends",__FILE__, __LINE__));
}
extern "C" void expose_plugin(v8::Isolate* isolate) {
	slim::plugin::plugin path_plugin(isolate, "path");
	path_plugin.add_function("join", slim::plugin::path::join);
	path_plugin.expose_plugin();
	return;
}

/*

process !== "undefined" && !!process.nextTick && !process.browser

https://nodejs.org/en/learn/asynchronous-work/understanding-processnexttick

*/