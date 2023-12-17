#ifndef __SLIM__PLUGIN__LOADER__HPP
#define __SLIM__PLUGIN__LOADER__HPP
#include <v8.h>
#include <unordered_map>
#include <slim/path.h>
namespace slim::plugin::loader {
    void load(const v8::FunctionCallbackInfo<v8::Value>& args);
    void load_plugin(v8::Isolate* isolate, const std::string plugin_name, const bool global_scope);
}
#endif