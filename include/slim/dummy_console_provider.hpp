#ifndef __SLIM_DUMMY_CONSOLE_PROVIDER__HPP
#define __SLIM_DUMMY_CONSOLE_PROVIDER__HPP
#include <v8.h>
#include <slim/plugin.hpp>
class DummyConsoleProvider {
	static void noContent(const v8::FunctionCallbackInfo<v8::Value>& args);
public:
	DummyConsoleProvider();
	~DummyConsoleProvider();
	void expose_plugin(v8::Isolate* isolate);
};
DummyConsoleProvider::DummyConsoleProvider(){}
DummyConsoleProvider::~DummyConsoleProvider(){}
void DummyConsoleProvider::noContent(const v8::FunctionCallbackInfo<v8::Value>& args){}
void DummyConsoleProvider::expose_plugin(v8::Isolate* isolate) {
	slim::plugin::plugin dummy_console_plugin(isolate, "console");
	dummy_console_plugin.add_function("count", &this->noContent);
 	dummy_console_plugin.add_function("countReset", &this->noContent);
	dummy_console_plugin.add_function("group", &this->noContent);
	dummy_console_plugin.add_function("groupCollapsed", &this->noContent);
	dummy_console_plugin.add_function("groupEnd", &this->noContent);
	dummy_console_plugin.add_function("time", &this->noContent);
	dummy_console_plugin.add_function("timeLog", &this->noContent);
	dummy_console_plugin.add_function("timeEnd", &this->noContent);
	dummy_console_plugin.add_function("assert", &this->noContent);
	dummy_console_plugin.add_function("clear", &this->noContent);
	dummy_console_plugin.add_function("debug", &this->noContent);
	dummy_console_plugin.add_function("dir", &this->noContent);
	dummy_console_plugin.add_function("dirxml", &this->noContent);
	dummy_console_plugin.add_function("error", &this->noContent);
	dummy_console_plugin.add_function("info", &this->noContent);
	dummy_console_plugin.add_function("log", &this->noContent);
	dummy_console_plugin.add_function("print", &this->noContent);
	dummy_console_plugin.add_function("table", &this->noContent);
	dummy_console_plugin.add_function("trace", &this->noContent);
	dummy_console_plugin.add_function("warn", &this->noContent);
	dummy_console_plugin.expose_plugin();
	return;
}
#endif