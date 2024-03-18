#include <v8.h>
#include <slim/gv8.h>
#include <slim/plugin.hpp>
#include <slim/utilities.h>
namespace slim::process {
    v8::Isolate* process_isolate = slim::gv8::GetIsolate();
}
extern "C" void expose_plugin(v8::Isolate* isolate) {
	slim::plugin::plugin process_plugin(isolate, "process");
	process_plugin.expose_plugin();
	return;
}
