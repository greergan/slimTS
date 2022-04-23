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
    static void SerializationFunctionTemplate(const v8::FunctionCallbackInfo<v8::Value>& args) {
        args.GetReturnValue().Set(args[0]);
    }
    template<typename Function>
    v8::Local<v8::FunctionTemplate> wrap_function_template(v8::Isolate* isolate, Function&& function) {
        using Function_type = typename std::decay<Function>::type;
        return v8::FunctionTemplate::New(isolate, &detail::forward_function<Traits, Function_type>,
                                            detail::external_data::set(isolate, std::forward<Function_type>(function)));
    }

    void expose(v8::Isolate* isolate) {
/*         v8pp::module console_module(isolate);
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
 */
/*         v8pp::module slim_module(isolate);
        slim_module.set("http", &slim::http::start);
        isolate->GetCurrentContext()->Global()->Set(isolate->GetCurrentContext(), v8pp::to_v8(isolate, "slim"), slim_module.new_instance()).ToChecked(); */
        //v8::Local<v8::ObjectTemplate> console_template = v8::ObjectTemplate::New(slim::veight::isolate);
        //v8::Local<v8::FunctionTemplate> console_methods = v8::FunctionTemplate::New(isolate, SerializationFunctionTemplate);
/*         console_template->Set(isolate, "clear", slim::console::clear);
        v8::Local<v8::Data> =
        slim::veight::global->Set(isolate, "console", console_template); */
    }
}
#endif