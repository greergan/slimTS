#ifndef __SLIM__OBJECTS__HPP
#define __SLIM__OBJECTS__HPP
#include <v8.h>
#include <slim/dummy_console_provider.hpp>
#include <slim/plugin.hpp>
#include <slim/plugin/loader.hpp>
namespace slim::objects {
	void initialize(v8::Isolate* isolate);
}
void slim::objects::initialize(v8::Isolate* isolate) {
	//slim::dummy_console::expose_plugin(isolate);
	slim::plugin::plugin slim_objects(isolate, "slim");
	slim_objects.add_function("load", &slim::plugin::loader::load);
	slim_objects.expose_plugin();
}
#endif