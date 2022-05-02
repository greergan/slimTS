#ifndef __SLIM__MODULE__HPP
#define __SLIM__MODULE__HPP
#include <functional>
#include <variant>
#include <v8.h>
#include <utilities.hpp>
namespace slim::module {
    class PropertyPointer {
        std::variant<bool*, std::string*, int*> property;
        public:
            PropertyPointer(auto&& property): property{property} {}
            ~PropertyPointer() {}
            v8::Local<v8::Value> GetV8Value(v8::Isolate* isolate) {
                if(std::holds_alternative<std::string*>(property)) {
                    return slim::utilities::StringToString(isolate, *std::get<std::string*>(property)).As<v8::Value>();
                }
                else if(std::holds_alternative<bool*>(property)) {
                    return v8::Boolean::New(isolate, *std::get<bool*>(property)).As<v8::Value>();
                }
                else if(std::holds_alternative<int*>(property)) {
                    return v8::Int32::New(isolate, *std::get<int*>(property)).As<v8::Value>();
                }
                return slim::utilities::StringToString(isolate, "").As<v8::Value>(); 
            }
            void SetValue(v8::Isolate* isolate, v8::Local<v8::Value> value) {
                if(std::holds_alternative<std::string*>(property)) {
                    std::get<std::string*>(property)->assign(slim::utilities::SlimColorValue(isolate, value));
                }
                else if(std::holds_alternative<bool*>(property)) {
                    *std::get<bool*>(property) = slim::utilities::SlimBoolValue(isolate, value);
                }
                else if(std::holds_alternative<int*>(property)) {
                    *std::get<int*>(property) = slim::utilities::SlimIntValue(isolate, value);
                }
            }
    };
};
namespace slim::module {
    struct module {
        module(v8::Isolate* isolate, std::string name): isolate{isolate} {
            module_template = v8::ObjectTemplate::New(isolate);
            v8_name = slim::utilities::StringToName(isolate, name);
        }
        template<typename Function, typename Signature = typename std::decay<Function>::type>
        void add_function(std::string name, Function&& function) {
            v8::HandleScope scope(isolate);
            module_template->Set(slim::utilities::StringToName(isolate, name), v8::FunctionTemplate::New(isolate, std::forward<Signature>(function)));
            /*
                * SetCallAsFunctionHandler
                *
                * Sort out how to wrap signature-less functions
                * v8::Local<v8::External>::New(isolate, value->pext);
                * 
                * Sort out how to wrap lambda's
                * log_module.set("warn", [&](const v8::FunctionCallbackInfo<v8::Value>& args){slim::log::warn(args);});
                * slim_module.set("http", [&](const v8::FunctionCallbackInfo<v8::Value>& args){slim::http::start(args);});
                * 
                */
        }
        void add_module(std::string name, module* submodule) {
            v8::HandleScope scope(isolate);
            module_template->Set(slim::utilities::StringToName(isolate, name), submodule->module_template);
        }
        void add_property(std::string name, auto&& property) {
            v8::HandleScope scope(isolate);
            auto getter = [](v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info){
                v8::Handle<v8::External> data = v8::Handle<v8::External>::Cast(info.Data());
                info.GetReturnValue().Set(static_cast<slim::module::PropertyPointer*>(data->Value())->GetV8Value(info.GetIsolate()));
            };
            auto setter = [](v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
                    v8::Handle<v8::External> data = v8::Handle<v8::External>::Cast(info.Data());
                    static_cast<slim::module::PropertyPointer*>(data->Value())->SetValue(info.GetIsolate(), value);
            };
            PropertyPointer* property_pointer = new PropertyPointer(property);
            module_template->SetAccessor(slim::utilities::StringToName(isolate, name), getter, setter, v8::External::New(isolate, (void*)property_pointer));
        }
        void expose_module() {
            isolate->GetCurrentContext()->Global()->Set(isolate->GetCurrentContext(), v8_name, new_instance()).ToChecked();
        }
        v8::Local<v8::ObjectTemplate> get_module() {
            return module_template;
        }
        v8::Local<v8::Object> new_instance() {
            return module_template->NewInstance(isolate->GetCurrentContext()).ToLocalChecked();
        }
        private:
            v8::Isolate* isolate;
            v8::Local<v8::Name> v8_name;
            v8::Local<v8::ObjectTemplate> module_template;
    };
};
#endif