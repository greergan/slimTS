#ifndef __SLIM__UTILITIES__HPP
#define __SLIM__UTILITIES__HPP
#include <string>
#include <v8.h>
namespace slim::utilities {
    template <typename T>
    v8::Local<v8::Object> GetObject(v8::Isolate* isolate, T object);
    v8::Local<v8::Object> GetObject(v8::Isolate* isolate, std::string string, v8::Local<v8::Object> object);
    v8::Local<v8::Value> GetValue(v8::Isolate* isolate, std::string string, v8::Local<v8::Object> object);
    int IntValue(v8::Isolate* isolate, std::string string, v8::Local<v8::Object> object);
    double NumberValue(v8::Isolate* isolate, v8::Local<v8::Value> value);
    int PropertyCount(v8::Isolate *isolate, v8::Local<v8::Object> object);
    std::string StringFunction(v8::Isolate* isolate, v8::Local<v8::Function> function);
    v8::Local<v8::Name> StringToName(v8::Isolate* isolate, std::string string);
    std::string StringValue(v8::Isolate* isolate, v8::Local<v8::Value> value);
    std::string StringValue(v8::Isolate* isolate, std::string string, v8::Local<v8::Object> object);

    template <typename T>
    v8::Local<v8::Object> GetObject(v8::Isolate* isolate, T object) {
        return object->ToObject(isolate->GetCurrentContext()).ToLocalChecked();
    }
    v8::Local<v8::Object> GetObject(v8::Isolate* isolate, std::string string, v8::Local<v8::Object> object) {
        return GetObject(isolate, GetValue(isolate, string, object));
    }
    v8::Local<v8::Value> GetValue(v8::Isolate* isolate, std::string string, v8::Local<v8::Object> object) {
        return object->Get(isolate->GetCurrentContext(), StringToName(isolate, string)).ToLocalChecked();
    }
    int IntValuePositive(v8::Isolate* isolate, std::string string, v8::Local<v8::Object> object) {
        v8::Local<v8::Value> value = GetValue(isolate, string, object);
        if(value->IsInt32()) {
            return value->Int32Value(isolate->GetCurrentContext()).FromJust();
        }
        return -1;
    }
    double NumberValue(v8::Isolate* isolate, v8::Local<v8::Value> value) {
        return value->NumberValue(isolate->GetCurrentContext()).FromJust();
    }
    int PropertyCount(v8::Isolate *isolate, v8::Local<v8::Object> object) {
        return object->GetPropertyNames(isolate->GetCurrentContext()).ToLocalChecked()->Length();
    }
    std::string StringFunction(v8::Isolate* isolate, v8::Local<v8::Value> function) {
        return StringValue(isolate, function->ToString(isolate->GetCurrentContext()).ToLocalChecked());
    }
    v8::Local<v8::Name> StringToName(v8::Isolate* isolate, std::string string) {
        return v8::String::NewFromUtf8(isolate, string.c_str()).ToLocalChecked();
    }
    std::string StringValue(v8::Isolate* isolate, v8::Local<v8::Value> value) {
        v8::String::Utf8Value utf8_value(isolate, value);
        return std::string(*utf8_value);
    }
    std::string StringValue(v8::Isolate* isolate, std::string string, v8::Local<v8::Object> object) {
        return StringValue(isolate, GetValue(isolate, string, object));
    }
}
#endif