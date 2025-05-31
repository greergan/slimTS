#include <dlfcn.h>
#include <v8.h>
#include <unordered_map>
#include <slim/common/log.h>
#include <slim/path.h>
#include <slim/utilities.h>
#include <slim/plugin/loader.h>
namespace slim::plugin::loader {
    std::string plugin_library_path = slim::path::getExecutableDir() + "/../lib/slimTS/";
    std::unordered_map<std::string, void*> loaded_plugins = {};
}
void slim::plugin::loader::load(const v8::FunctionCallbackInfo<v8::Value>& args) {
    auto isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);
    if(args.Length() == 0) {
        isolate->ThrowException(slim::utilities::StringToValue(isolate, "slim.load(plugin_name, [bool]"));
    }
    if(!args[0]->IsString()) {
        isolate->ThrowException(slim::utilities::StringToValue(isolate, "argument 1 must be a string"));
    }
    if(args.Length() > 1 && !args[1]->IsBoolean()) {
        isolate->ThrowException(slim::utilities::StringToValue(isolate, "argument 2 must be true or false"));
    }
    auto plugin_name = slim::utilities::v8ValueToString(isolate, args[0]);
    auto global_scope = args.Length() > 1 && args[1]->IsBoolean() ? args[1]->BooleanValue(isolate) : false;
    load_plugin(isolate, plugin_name, global_scope);
    return;
};
void slim::plugin::loader::load_plugin(v8::Isolate* isolate, const std::string plugin_name_string, const bool global_scope) {
    slim::common::log::trace(slim::common::log::Message("slim::plugin::loader", "begins", __FILE__, __LINE__));
    auto open_bits = global_scope ? RTLD_NOW | RTLD_GLOBAL : RTLD_NOW;
    std::string plugin_so_path = plugin_library_path + plugin_name_string + ".so";
    if(true) {
        slim::common::log::debug(slim::common::log::Message("slim::plugin::loader", std::string("loading plugin => " + plugin_name_string).c_str(), __FILE__, __LINE__));
        loaded_plugins[plugin_name_string] = dlopen(plugin_so_path.c_str(), open_bits);
        if(!loaded_plugins[plugin_name_string]) {
            isolate->ThrowException(slim::utilities::StringToValue(isolate, "error loading plugin: " + std::string(dlerror())));
        }
        else {
            typedef void (*expose_plugin_t)(v8::Isolate* isolate);
            expose_plugin_t expose_plugin = (expose_plugin_t) dlsym(loaded_plugins[plugin_name_string], "expose_plugin");
            dlerror();
            if(!expose_plugin) {
                isolate->ThrowException(slim::utilities::StringToValue(isolate, "error loading plugin symbols: " + std::string(dlerror())));
                dlclose(loaded_plugins[plugin_name_string]);
            }
            else {
                slim::common::log::debug(slim::common::log::Message("slim::plugin::loader", std::string("exposing plugin => " + plugin_name_string).c_str(), __FILE__, __LINE__));
                expose_plugin(isolate);
                expose_plugin = nullptr;
            }
        }
    }
    else {
        slim::common::log::debug(slim::common::log::Message("slim::plugin::loader", "plugin already loaded", __FILE__, __LINE__));
    }
    slim::common::log::trace(slim::common::log::Message("slim::plugin::loader", "ends", __FILE__, __LINE__));
}