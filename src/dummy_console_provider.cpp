#include <slim/dummy_console_provider.h>
void slim::dummy_console::expose_plugin(v8::Isolate* isolate) {
	auto no_content = [](const v8::FunctionCallbackInfo<v8::Value>& args){};
	slim::plugin::plugin dummy_console_plugin(isolate, "console");
	dummy_console_plugin.add_function("assert",         no_content);
	dummy_console_plugin.add_function("clear",          no_content);
	dummy_console_plugin.add_function("count",          no_content);
 	dummy_console_plugin.add_function("countReset",     no_content);
	dummy_console_plugin.add_function("debug",          no_content);
	dummy_console_plugin.add_function("dir",            no_content);
	dummy_console_plugin.add_function("dirxml",         no_content);
	dummy_console_plugin.add_function("error",          no_content);
	dummy_console_plugin.add_function("group",          no_content);
	dummy_console_plugin.add_function("groupCollapsed", no_content);
	dummy_console_plugin.add_function("groupEnd",       no_content);
	dummy_console_plugin.add_function("info",           no_content);
	dummy_console_plugin.add_function("listen",         no_content);
	dummy_console_plugin.add_function("log",            no_content);
	dummy_console_plugin.add_function("print",          no_content);
	dummy_console_plugin.add_function("table",          no_content);
	dummy_console_plugin.add_function("time",           no_content);
	dummy_console_plugin.add_function("timeEnd",        no_content);
	dummy_console_plugin.add_function("timeLog",        no_content);
	dummy_console_plugin.add_function("todo",           no_content);
	dummy_console_plugin.add_function("trace",          no_content);
	dummy_console_plugin.add_function("warn",           no_content);
	dummy_console_plugin.add_function("write",          no_content);
	dummy_console_plugin.expose_plugin();
}