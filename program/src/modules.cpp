#include <module.h>
#include <modules.h>
void slim::modules::Expose(v8::Isolate* isolate) {
    //ExposeConsole(isolate);
    //ExposePoint(isolate);
/*         v8pp::module log_module(isolate);
        log_module.set("warn", [&](const v8::FunctionCallbackInfo<v8::Value>& args){slim::log::warn(args);}); */

    //v8pp::module slim_module(isolate);
    //slim_module.set("http", [&](const v8::FunctionCallbackInfo<v8::Value>& args){slim::http::start(args);});
    //slim_module.set("log", log_module);
    //isolate->GetCurrentContext()->Global()->Set(isolate->GetCurrentContext(), slim::utilities::StringToName(isolate, "slim"), slim_module.new_instance()).ToChecked();
}