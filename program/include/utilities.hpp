#ifndef __SLIM__UTILITIES__HPP
#define __SLIM__UTILITIES__HPP
#include <v8.h>
#include <string>
#include <string_view>
namespace slim::utilities {
    int ArrayCount(v8::Local<v8::Value> value);
    template <typename Thing>
    v8::Local<v8::Object> GetObject(v8::Isolate* isolate, Thing object);
    v8::Local<v8::Object> GetObject(v8::Isolate* isolate, std::string string, v8::Local<v8::Object> object);
    v8::Local<v8::Value> GetValue(v8::Isolate* isolate, std::string string, v8::Local<v8::Object> object);
    int IntValue(v8::Isolate* isolate, v8::Local<v8::Value> value);
    int IntValue(v8::Isolate* isolate, std::string string, v8::Local<v8::Object> object);
    double NumberValue(v8::Isolate* isolate, v8::Local<v8::Value> value);
    int PropertyCount(v8::Isolate* isolate, v8::Local<v8::Object> object);
    std::string ScriptFileName(v8::Local<v8::Message> message);
    int ScriptLineNumber(v8::Local<v8::Message> message);
    std::string ScriptLine(v8::Local<v8::Message> message);
    std::string ScriptStackTrace(v8::TryCatch* try_catch);
    std::string StringFunction(v8::Isolate* isolate, v8::Local<v8::Function> function);
    v8::Local<v8::Name> StringToName(v8::Isolate* isolate, std::string string);
    v8::Local<v8::String> StringToString(v8::Isolate* isolate, std::string string);
    v8::Local<v8::Value> StringToValue(v8::Isolate* isolate, std::string string);
    template <typename Thing>
    std::string StringValue(v8::Isolate* isolate, Thing thingy);
    std::string StringValue(v8::Isolate* isolate, std::string string, v8::Local<v8::Object> object);
    template <typename Thing>
    std::string_view StringViewValue(v8::Isolate* isolate, Thing thingy);
    std::string_view StringViewValue(v8::Isolate* isolate, std::string string, v8::Local<v8::Object> object);
    int ArrayCount(v8::Local<v8::Value> value) {
        if(value->IsArray()) {
            return v8::Handle<v8::Array>::Cast(value)->Length();
        }
        return 0;
    }
    template <typename Thing>
    v8::Local<v8::Object> GetObject(v8::Isolate* isolate, Thing thingy) {
        return (thingy->IsObject()) ? thingy->ToObject(isolate->GetCurrentContext()).ToLocalChecked() : v8::Object::New(isolate);
    }
    v8::Local<v8::Object> GetObject(v8::Isolate* isolate, std::string string, v8::Local<v8::Object> object) {
        return GetObject(isolate, GetValue(isolate, string, object));
    }
    v8::Local<v8::Value> GetValue(v8::Isolate* isolate, std::string string, v8::Local<v8::Object> object) {
        return object->Get(isolate->GetCurrentContext(), StringToName(isolate, string)).ToLocalChecked();
    }
    int IntValue(v8::Isolate* isolate, v8::Local<v8::Value> value) {
        return value->Int32Value(isolate->GetCurrentContext()).FromJust();
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
    std::string ScriptFileName(v8::Local<v8::Message> message) {
        return StringValue(message->GetIsolate(), message->GetScriptOrigin().ResourceName());
    }
    std::string ScriptLine(v8::Local<v8::Message> message) {
         return StringValue(message->GetIsolate(), message->GetSourceLine(message->GetIsolate()->GetCurrentContext()).ToLocalChecked());
    }
    int ScriptLineNumber(v8::Local<v8::Message> message) {
        return message->GetLineNumber(message->GetIsolate()->GetCurrentContext()).FromJust();
    }
    std::string ScriptStackTrace(v8::TryCatch* try_catch) {
        auto isolate = try_catch->Message()->GetIsolate();
        return StringValue(isolate, try_catch->StackTrace(isolate->GetCurrentContext()).ToLocalChecked());
    }
    std::string StringFunction(v8::Isolate* isolate, v8::Local<v8::Value> function) {
        return StringValue(isolate, function->ToString(isolate->GetCurrentContext()).ToLocalChecked());
    }
    v8::Local<v8::Name> StringToName(v8::Isolate* isolate, std::string string) {
        return v8::String::NewFromUtf8(isolate, string.c_str()).ToLocalChecked();
    }
    v8::Local<v8::String> StringToString(v8::Isolate* isolate, std::string string) {
        return v8::String::NewFromUtf8(isolate, string.c_str()).ToLocalChecked();
    }
    v8::Local<v8::Value> StringToValue(v8::Isolate* isolate, std::string string) {
        return v8::String::NewFromUtf8(isolate, string.c_str()).ToLocalChecked();
    }
    template <typename Thing>
    std::string StringValue(v8::Isolate* isolate, Thing thingy) {
        v8::String::Utf8Value utf8_value(isolate, thingy);
        return std::string(*utf8_value);
    }
    std::string StringValue(v8::Isolate* isolate, std::string string, v8::Local<v8::Object> object) {
        return StringValue(isolate, GetValue(isolate, string, object));
    }
    template <typename Thing>
    std::string_view StringViewValue(v8::Isolate* isolate, Thing thingy) {
        v8::String::Utf8Value utf8_value(isolate, thingy);
        return std::string_view(*utf8_value);
    }
    std::string_view StringViewValue(v8::Isolate* isolate, std::string string, v8::Local<v8::Object> object) {
        return StringViewValue(isolate, GetValue(isolate, string, object));
    }
}
#endif