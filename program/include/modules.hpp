#ifndef __SLIM__EXPOSE__HPP
#define __SLIM__EXPOSE__HPP
#include <type_traits>
#include <string_view>
#include <functional>
#include <v8.h>
#include <veight.hpp>
#include <log.hpp>
#include <console.hpp>
#include <http_server.hpp>
namespace slim::modules {
    void ExposeConsole(v8::Isolate* isolate) {
        v8pp::module console_module(isolate);
        console_module.set("assert", &slim::console::console_assert);
        console_module.set("configure", &slim::console::configure);
        console_module.set("clear", &slim::console::clear);
        console_module.set("debug", &slim::console::debug);
        console_module.set("dir", &slim::console::dir);
        console_module.set("error", &slim::console::error);
        console_module.set("info", &slim::console::info);
        console_module.set("log", &slim::console::log);
        console_module.set("todo", &slim::console::todo);
        console_module.set("trace", &slim::console::trace);
        console_module.set("warn", &slim::console::warn);
        isolate->GetCurrentContext()->Global()->Set(isolate->GetCurrentContext(), v8pp::to_v8(isolate, "console"), console_module.new_instance()).ToChecked();
    }
    void expose(v8::Isolate* isolate) {
        ExposeConsole(isolate);
        v8pp::module log_module(isolate);
        log_module.set("warn", [&](const v8::FunctionCallbackInfo<v8::Value>& args){slim::log::warn(args);});

        v8pp::module slim_module(isolate);
        slim_module.set("http", [&](const v8::FunctionCallbackInfo<v8::Value>& args){slim::http::start(args);});
        slim_module.set("log", log_module);
        isolate->GetCurrentContext()->Global()->Set(isolate->GetCurrentContext(), v8pp::to_v8(isolate, "slim"), slim_module.new_instance()).ToChecked();
    }
}
#endif