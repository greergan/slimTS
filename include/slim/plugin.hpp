#ifndef __SLIM__PLUGIN__HPP
#define __SLIM__PLUGIN__HPP
#include <functional>
#include <variant>
#include <v8.h>
#include <slim/utilities.h>
namespace slim::plugin {
    class PropertyPointer {
        std::variant<bool*, std::string*, int*> property;
        public:
            PropertyPointer(auto&& property): property{property} {}
            ~PropertyPointer() {}
            v8::Local<v8::Value> GetV8Value(v8::Isolate* isolate) {
                v8::Local<v8::Value> value;
                if(std::holds_alternative<std::string*>(property)) {
                    value = slim::utilities::StringToString(isolate, *std::get<std::string*>(property)).As<v8::Value>();
                }
                else if(std::holds_alternative<bool*>(property)) {
                    value = v8::Boolean::New(isolate, *std::get<bool*>(property)).As<v8::Value>();
                }
                else if(std::holds_alternative<int*>(property)) {
                    value = v8::Int32::New(isolate, *std::get<int*>(property)).As<v8::Value>();
                }
                return value; 
            }
            void SetValue(v8::Isolate* isolate, v8::Local<v8::Value> value) {
                if(std::holds_alternative<std::string*>(property)) {
                    std::get<std::string*>(property)->assign(slim::utilities::StringValue(isolate, value));
                }
                else if(std::holds_alternative<bool*>(property)) {
                    *std::get<bool*>(property) = slim::utilities::BoolValue(isolate, value);
                }
                else if(std::holds_alternative<int*>(property)) {
                    *std::get<int*>(property) = slim::utilities::IntValue(isolate, value);
                }
            }
    };
    struct plugin {
        plugin(v8::Isolate* isolate, std::string name): isolate{isolate} {
            plugin_template = v8::ObjectTemplate::New(isolate);
            v8_name = slim::utilities::StringToName(isolate, name);
        }
        template<typename Function, typename Signature = typename std::decay<Function>::type>
        void add_function(std::string name, Function&& function) {
            v8::HandleScope scope(isolate);
            plugin_template->Set(slim::utilities::StringToName(isolate, name), v8::FunctionTemplate::New(isolate, std::forward<Signature>(function)));
        }
        void add_plugin(std::string name, plugin* subplugin) {
            v8::HandleScope scope(isolate);
            plugin_template->Set(slim::utilities::StringToName(isolate, name), subplugin->plugin_template);
        }
        void add_property(std::string name, auto&& property) {
            v8::HandleScope scope(isolate);
            auto getter = [](v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args){
                v8::Handle<v8::External> data = v8::Handle<v8::External>::Cast(args.Data());
                args.GetReturnValue().Set(static_cast<slim::plugin::PropertyPointer*>(data->Value())->GetV8Value(args.GetIsolate()));
            };
            auto setter = [](v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
                    v8::Handle<v8::External> data = v8::Handle<v8::External>::Cast(args.Data());
                    static_cast<slim::plugin::PropertyPointer*>(data->Value())->SetValue(args.GetIsolate(), value);
            };
            PropertyPointer* property_pointer = new PropertyPointer(property);
            plugin_template->SetAccessor(slim::utilities::StringToName(isolate, name), getter, setter, v8::External::New(isolate, (void*)property_pointer));
        }
        void expose_plugin() {
            isolate->GetCurrentContext()->Global()->Set(isolate->GetCurrentContext(), v8_name, new_instance()).ToChecked();
        }
        v8::Local<v8::ObjectTemplate> get_plugin() {
            return plugin_template;
        }
        v8::Local<v8::Object> new_instance() {
            return plugin_template->NewInstance(isolate->GetCurrentContext()).ToLocalChecked();
        }
        private:
            v8::Isolate* isolate;
            v8::Local<v8::Name> v8_name;
            v8::Local<v8::ObjectTemplate> plugin_template;
    };
}
#endif
