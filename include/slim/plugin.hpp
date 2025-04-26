#ifndef __SLIM__PLUGIN__HPP
#define __SLIM__PLUGIN__HPP
#include <functional>
#include <variant>
#include <v8.h>
#include <slim/utilities.h>

#define BOOL 1
#define CHAR 2
#define DOUBLE 3
#define FLOAT 4
#define INTEGER 5
#define NUMBER 6
#define STRING 7
#include <iostream>

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
        void add_plugin(std::string name, plugin* child) {
            v8::HandleScope scope(isolate);
            plugin_template->Set(slim::utilities::StringToName(isolate, name), child->plugin_template);
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
            plugin_template->SetNativeDataProperty(slim::utilities::StringToName(isolate, name), getter, setter, v8::External::New(isolate, (void*)property_pointer));
        }
        void add_property_immutable(const char* property_name, const char* property_value) {
            plugin_template->Set(slim::utilities::StringToV8String(isolate, property_name), slim::utilities::StringToV8String(isolate, property_value), v8::ReadOnly);
        }
/*      
Figure out how to make a template work so we can remove the overload method calls

        template<typename T, typename U>
        void add_property_immutable(T property_name, U property_value) {
            v8::HandleScope scope(isolate);
            std::string property_name_string;
            v8::Local<v8::Value> v8_property_value;
            auto get_v8_data_type = [&]<typename X>(X&& property_name_or_value_trait) -> int {
                using removed_reference = std::remove_reference<X>::type;
                using removed_pointer = std::remove_pointer<removed_reference>::type;
                using removed_const = std::remove_const<removed_pointer>::type;
                if(std::is_same_v<bool, removed_const>) {
                    return BOOL;
                }
                else if(std::is_same_v<int, removed_const>) {
                    return INTEGER;
                }
                else if(std::is_same_v<char, removed_const>) {
                    return CHAR;
                }
                else if(std::is_same_v<double, removed_const> || std::is_same_v<float, removed_const>) {
                    return NUMBER;
                }
                else if(std::is_same_v<std::string, removed_const>) {
                    return STRING;
                }
                return -1;
            };
            int property_name_data_type = get_v8_data_type(std::type_identity_t<U>);
            if (property_name_data_type == CHAR) {
                property_name_string = std::string(property_name);
            }
            else if(property_name_data_type == STRING) {
                property_name_string = property_name;
            }
            else {
                throw std::invalid_argument("slim::plugin::plugin::add_property_immutable requires a string or char type for the property name");
            }
            int property_value_data_type = get_v8_data_type(std::type_identity_t<T>);
            if (property_value_data_type == BOOL) {
                plugin_template->Set(slim::utilities::StringToV8String(isolate, property_name_string), v8::Boolean::New(isolate, property_value), v8::ReadOnly);
            }
            else if(property_value_data_type == CHAR || property_value_data_type == STRING) {
                plugin_template->Set(slim::utilities::StringToV8String(isolate, property_name_string), slim::utilities::StringToV8String(isolate, property_value), v8::ReadOnly);
            }
            else if(property_value_data_type == INTEGER) {
                plugin_template->Set(slim::utilities::StringToV8String(isolate, property_name_string), v8::Integer::New(isolate, property_value), v8::ReadOnly);
            }
            else if(property_value_data_type == NUMBER) {
                plugin_template->Set(slim::utilities::StringToV8String(isolate, property_name_string), v8::Number::New(isolate, property_value), v8::ReadOnly);
            }
            else {
                throw std::invalid_argument("slim::plugin::plugin::add_property_immutable requires a primitive type for the property value");
            }
        } */
        void expose_plugin(v8::Local<v8::Object> new_v8_object) {
            isolate->GetCurrentContext()->Global()->Set(isolate->GetCurrentContext(), v8_name, new_v8_object).ToChecked();
        }
        void expose_plugin() {
            expose_plugin(new_instance());
        }
        v8::Local<v8::ObjectTemplate>& get_plugin() {
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
