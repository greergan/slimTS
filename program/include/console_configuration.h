#ifndef __SLIM__CONSOLE__INCLUDES
#define __SLIM__CONSOLE__INCLUDES
#include <unordered_map>
#include <string>
#define __SLIM__CONSOLE__CONFIGURATION__H
#define __SLIM__CONSOLE__CONFIGURATION__H
#include <console.h>
#include <v8.h>
namespace slim::console::configuration {
    extern slim::console::Configuration dir, log, debug, error, info, todo, trace, warn;
    extern std::unordered_map<std::string, slim::console::Configuration*> configurations;
    extern void copy(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void copy(const slim::console::BaseConfiguration* source, slim::console::BaseConfiguration* destination);
    extern void configure(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void configure(v8::Isolate* isolate, const v8::Local<v8::Object> object, slim::console::BaseConfiguration* configuration);
};
#endif