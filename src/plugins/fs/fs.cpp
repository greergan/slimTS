#include <slim/gv8.h>
#include <slim/plugin.hpp>
namespace slim::fs {
    v8::Isolate* fs_isolate = slim::gv8::GetIsolate();
}
extern "C" void expose_plugin(v8::Isolate* isolate) {
	slim::plugin::plugin fs_plugin(isolate, "fs");
	fs_plugin.expose_plugin();
	return;
}
