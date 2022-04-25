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
#include <module_accessors.hpp>

/*
 *  https://v8docs.nodesource.com/node-16.13/index.html
 *
 * 
 * static Local<ObjectTemplate> New (Isolate* isolate,
 *                                    Local< FunctionTemplate > constructor = Local< FunctionTemplate >() ) 	
 * 
 */

namespace slim::utilities {
    struct module {
        private:
            v8::Isolate* isolate;
            v8::Local<v8::Name> v8_name;
            v8::Local<v8::ObjectTemplate> module_template;
            v8::Local<v8::Object> new_instance() {
                return module_template->NewInstance(isolate->GetCurrentContext()).ToLocalChecked();
            }
        public:
            explicit module(v8::Isolate* isolate, std::string name) : isolate{isolate} {
                module_template = v8::ObjectTemplate::New(isolate);
                module_template->SetInternalFieldCount(1);
                v8_name = slim::utilities::StringToName(isolate, name);
            }
            module(module const&) = delete;
	        module& operator=(module const&) = delete;
	        module(module&&) = default;
	        module& operator=(module&&) = default;
            void add_module(std::string name, module* submodule) {
                v8::HandleScope scope(isolate);
                module_template->Set(slim::utilities::StringToName(isolate, name), submodule->module_template);
            }
            void expose_module() {
                isolate->GetCurrentContext()->Global()->Set(isolate->GetCurrentContext(), v8_name, new_instance()).ToChecked();
            }
            v8::Local<v8::ObjectTemplate> get_module() {
                return module_template;
            }
            template<typename Function, typename Signature = typename std::decay<Function>::type>
            void set(std::string name, Function&& function) {
                v8::HandleScope scope(isolate);
                module_template->Set(slim::utilities::StringToName(isolate, name), v8::FunctionTemplate::New(isolate, std::forward<Signature>(function)));
                /*
                 * Sort out how to wrap signature-less functions
                 * v8::Local<v8::External>::New(isolate, value->pext);
                 * 
                 * Sort out how to wrap lambda's
                 * log_module.set("warn", [&](const v8::FunctionCallbackInfo<v8::Value>& args){slim::log::warn(args);});
                 * slim_module.set("http", [&](const v8::FunctionCallbackInfo<v8::Value>& args){slim::http::start(args);});
                 * 
                 */
                
            }
    };
};



namespace slim::modules {
    slim::utilities::module AssembleConsoleConfiguration(v8::Isolate* isolate);
    void ExposeConsole(v8::Isolate* isolate);
    slim::utilities::module AssembleConsoleConfiguration(v8::Isolate* isolate) {
        slim::utilities::module warn_module(isolate, "warn");
        warn_module
        //slim::modules::accessors::GetWarnTextColor

        slim::utilities::module configuration_module(isolate, "configuration");
        configuration_module.add_module("warn", &warn_module);
        return configuration_module;
    }
    void ExposeConsole(v8::Isolate* isolate) {
        slim::utilities::module configuration = AssembleConsoleConfiguration(isolate);
        slim::utilities::module console_module(isolate, "console");
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