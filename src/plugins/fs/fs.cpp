#include <slim/gv8.h>
#include <slim/plugin.hpp>
namespace slim::plugin::fs {
	namespace realpathSync {
		void native(const v8::FunctionCallbackInfo<v8::Value>& args);
	}
}
void slim::plugin::fs::realpathSync::native(const v8::FunctionCallbackInfo<v8::Value>& args) {
	//v8::Isolate* isolate = args.GetIsolate();
	//slim::utilities::BoolToV8Boolean(isolate, false);
	//args.GetReturnValue().Set(slim::utilities::BoolToV8Boolean(isolate, false));
}
extern "C" void expose_plugin(v8::Isolate* isolate) {
	slim::plugin::plugin fs_plugin(isolate, "fs");
	slim::plugin::plugin realpathSync_plugin(isolate, "realpathSync");
	realpathSync_plugin.add_function("native", slim::plugin::fs::realpathSync::native);
	fs_plugin.add_plugin("realpathSync", &realpathSync_plugin);
	fs_plugin.expose_plugin();
	return;
}
