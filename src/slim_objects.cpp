#include <v8.h>
#include <slim/objects.h>
#include <slim/dummy_console_provider.h>
#include <slim/fetch.h>
#include <slim/plugin.hpp>
#include <slim/plugin/loader.h>
void slim::objects::initialize(v8::Isolate* isolate) {
	slim::dummy_console::expose_plugin(isolate);
	slim::network::fetch::expose_plugin(isolate);
	slim::plugin::plugin slim_objects(isolate, "slim");
	slim_objects.add_function("load", &slim::plugin::loader::load);
	slim_objects.expose_plugin();
}
