#ifndef __SLIM__UTILITIES__H
#define __SLIM__UTILITIES__H
#include <string>
#include <v8.h>
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

    bool GetBoolValueOrException(v8::Isolate* isolate, v8::Local<v8::Value> value);
    std::string GetColorValueOrException(v8::Isolate* isolate, v8::Local<v8::Value> value);
    int GetIntValueOrException(v8::Isolate* isolate, v8::Local<v8::Value> value);

    std::string GetFunctionString(v8::Isolate* isolate, v8::Local<v8::Value> function);


    v8::Local<v8::Name> StringToName(v8::Isolate* isolate, std::string string);
    v8::Local<v8::String> StringToString(v8::Isolate* isolate, std::string string);
    v8::Local<v8::Value> StringToValue(v8::Isolate* isolate, std::string string);


    template <typename Thing>
    std::string StringValue(v8::Isolate* isolate, Thing thingy);
    std::string StringValue(v8::Isolate* isolate, std::string string, v8::Local<v8::Object> object);
    template <typename Thing>
    std::string_view StringViewValue(v8::Isolate* isolate, Thing thingy);
    std::string_view StringViewValue(v8::Isolate* isolate, std::string string, v8::Local<v8::Object> object);

    extern void HandleLibUVError(const char* message, int error);
};
#endif