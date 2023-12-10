#include <v8.h>
#include <slim/plugin.hpp>
namespace slim::console_provider {
	void internal_print(const v8::FunctionCallbackInfo<v8::Value>& args, const char* level);
	void debug(const v8::FunctionCallbackInfo<v8::Value>& args) { internal_print(args, "DEBUG"); }
	void dir(const v8::FunctionCallbackInfo<v8::Value>& args) { internal_print(args, ""); }
	void dirxml(const v8::FunctionCallbackInfo<v8::Value>& args) { std::puts("console.dirxml is not implemented"); }
	void error(const v8::FunctionCallbackInfo<v8::Value>& args) { internal_print(args, "ERROR"); }
	void info(const v8::FunctionCallbackInfo<v8::Value>& args) { internal_print(args, "INFO"); }
	void log(const v8::FunctionCallbackInfo<v8::Value>& args) { internal_print(args, ""); }
	void print(const v8::FunctionCallbackInfo<v8::Value>& args) { internal_print(args, ""); }
	void table(const v8::FunctionCallbackInfo<v8::Value>& args) { std::puts("console.table is not implemented"); }
	void trace(const v8::FunctionCallbackInfo<v8::Value>& args) { internal_print(args, "TRACE"); }
	void warn(const v8::FunctionCallbackInfo<v8::Value>& args) { internal_print(args, "WARN"); }
	void internal_print(const v8::FunctionCallbackInfo<v8::Value>& args, const char* level="") { 
		if(strlen(level))
			std::puts(level);
		else
			std::puts("no level given");
	}
};
extern "C" void expose_plugin(v8::Isolate* isolate) {
	slim::plugin::plugin console_plugin(isolate, "console");
	// console_plugin.add_function("assert", &slim::console_provider::assert);
	// console_plugin.add_function("clear", &slim::console_provider::clear);
	// console_plugin.add_function("count", &slim::console_provider::count);
 	// console_plugin.add_function("countReset", &slim::console_provider::countReset);
	// console_plugin.add_function("group", &slim::console_provider::group);
	// console_plugin.add_function("groupCollapsed", &slim::console_provider::groupCollapsed);
	// console_plugin.add_function("groupEnd", &slim::console_provider::groupEnd);
	// console_plugin.add_function("time", &slim::console_provider::time);
	// console_plugin.add_function("timeLog", &slim::console_provider::timeLog);
	// console_plugin.add_function("timeEnd", &slim::console_provider::timeEnd);
	console_plugin.add_function("debug", &slim::console_provider::debug);
	console_plugin.add_function("dir",   &slim::console_provider::dir);
	console_plugin.add_function("dirxml", &slim::console_provider::dirxml);
	console_plugin.add_function("error", &slim::console_provider::error);
	console_plugin.add_function("info",  &slim::console_provider::info);
	console_plugin.add_function("log",   &slim::console_provider::log);
	console_plugin.add_function("print", &slim::console_provider::print);
	console_plugin.add_function("table", &slim::console_provider::table);
	console_plugin.add_function("trace", &slim::console_provider::trace);
	console_plugin.add_function("warn",  &slim::console_provider::warn);
	console_plugin.expose_plugin();
	return;
}
