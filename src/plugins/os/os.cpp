#include <v8.h>
#include <slim/gv8.h>
#include <slim/plugin.hpp>
#include <slim/utilities.h>
namespace slim::os {
	//void exit_wrapper(const v8::FunctionCallbackInfo<v8::Value>& args);
}

}
extern "C" void expose_plugin(v8::Isolate* isolate) {
	slim::plugin::plugin os_plugin(isolate, "os");
	//process_plugin.add_property("browser", &slim::process::browser);
	//process_plugin.add_function("exit", slim::process::exit_wrapper);
	os_plugin.expose_plugin();
	return;
}

/*

process !== "undefined" && !!process.nextTick && !process.browser

https://nodejs.org/en/learn/asynchronous-work/understanding-processnexttick

*/