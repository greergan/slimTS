#ifndef __SLIM__MODULES__HPP
#define __SLIM__MODULES__HPP
#include <functional>
#include <v8.h>
#include <veight.hpp>
#include <log.hpp>
#include <console.hpp>
#include <fetch.hpp>
#include <http_server.hpp>
#include <utilities.hpp>
#include <module.hpp>

/*
 *  https://v8docs.nodesource.com/node-16.13/index.html
 *
 * 
 * static Local<ObjectTemplate> New (Isolate* isolate,
 *                                    Local< FunctionTemplate > constructor = Local< FunctionTemplate >() ) 	
 * 
 */
namespace slim::modules {
    slim::module::module AssembleConsoleConfiguration(v8::Isolate* isolate) {
        slim::module::module warn_module(isolate, "warn");
        warn_module.add_property("bold", &slim::console::configuration::warn.bold);
        warn_module.add_property("dim", &slim::console::configuration::warn.dim);
        warn_module.add_property("text_color", &slim::console::configuration::warn.text_color);
        //slim::modules::accessors::GetWarnTextColor

        slim::module::module configuration_module(isolate, "configuration");
        configuration_module.add_module("warn", &warn_module);
        return configuration_module;
    }
    void ExposeConsole(v8::Isolate* isolate) {
        slim::module::module configuration = AssembleConsoleConfiguration(isolate);
        slim::module::module console_module(isolate, "console");
        console_module.add_function("assert", &slim::console::console_assert);
        console_module.add_function("configure", &slim::console::configure);
        console_module.add_function("clear", &slim::console::clear);
        console_module.add_function("debug", &slim::console::debug);
        console_module.add_function("dir", &slim::console::dir);
        console_module.add_function("error", &slim::console::error);
        console_module.add_function("info", &slim::console::info);
        console_module.add_function("log", &slim::console::log);
        console_module.add_function("todo", &slim::console::todo);
        console_module.add_function("trace", &slim::console::trace);
        console_module.add_function("warn", &slim::console::warn);
        console_module.add_module("configuration", &configuration);
        console_module.expose_module();
    }

class Point {
 public:
  Point(int x=0, int y=0) : x_(x), y_(y) { }
  int x_, y_;
};

void GetPointX(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    int value = static_cast<Point*>(ptr)->x_;
    info.GetReturnValue().Set(value);
}
void SetPointX(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    static_cast<Point*>(ptr)->x_ = value->Int32Value(isolate->GetCurrentContext()).ToChecked();
}
    void ExposePoint(v8::Isolate* isolate) {
        v8::Local<v8::ObjectTemplate> point_templ = v8::ObjectTemplate::New(isolate /*, v8::<Local<v8::FunctionTemplate> constructor */);
        point_templ->SetInternalFieldCount(1);
        point_templ->SetAccessor(slim::utilities::StringToName(isolate, "x"), GetPointX, SetPointX);
        Point* point = new Point();
        v8::Local<v8::Object> obj = point_templ->NewInstance(isolate->GetCurrentContext()).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(isolate, point));
        isolate->GetCurrentContext()->Global()->Set(isolate->GetCurrentContext(), slim::utilities::StringToName(isolate, "Point"), obj).ToChecked();
    }
    void expose(v8::Isolate* isolate) {
        ExposeConsole(isolate);
        ExposePoint(isolate);
        v8pp::module log_module(isolate);
        log_module.set("warn", [&](const v8::FunctionCallbackInfo<v8::Value>& args){slim::log::warn(args);});

        v8pp::module slim_module(isolate);
        slim_module.set("http", [&](const v8::FunctionCallbackInfo<v8::Value>& args){slim::http::start(args);});
        slim_module.set("log", log_module);
        isolate->GetCurrentContext()->Global()->Set(isolate->GetCurrentContext(), slim::utilities::StringToName(isolate, "slim"), slim_module.new_instance()).ToChecked();
    }
}
#endif