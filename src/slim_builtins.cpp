#include <iostream>

#include <v8.h>
#include <slim/builtins.h>
#include <slim/builtins/dummy_console_provider.h>
#include <slim/builtins/typescript.h>
#include <slim/common/log.h>
#include <slim/common/memory_mapper.h>
#include <slim/gv8.h>
#include <slim/plugin.hpp>
#include <slim/plugin/loader.h>
/* look at source file product/google/v8/src/init/bootstrapper.cc */
namespace {
	using namespace slim::common;
	using namespace slim::utilities;
}
//void slim::builtins::initialize(v8::Isolate* isolate) {
void slim::builtins::initialize(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate>& globalObjectTemplate) {
	log::trace(log::Message("slim::builtins::initialize()","begins",__FILE__, __LINE__));
	//v8::HandleScope scope(isolate);
	log::trace(log::Message("slim::builtins::initialize()","after handle scope",__FILE__, __LINE__));
	slim::dummy_console::expose_plugin(isolate);
	log::trace(log::Message("slim::builtins::initialize()","after expose_plugin",__FILE__, __LINE__));
	slim::plugin::plugin slim_objects(isolate, "slim");
	log::trace(log::Message("slim::builtins::initialize()","after slim",__FILE__, __LINE__));
	slim_objects.add_function("load", slim::plugin::loader::load);
	log::trace(log::Message("slim::builtins::initialize()","after load",__FILE__, __LINE__));
	slim_objects.expose_plugin();
	log::trace(log::Message("slim::builtins::initialize()","after expose slim_objects",__FILE__, __LINE__));
	slim::builtins::typescript::initialize(isolate);
	memory_mapper::create("directories");
	log::trace(log::Message("slim::builtins::initialize()","ends",__FILE__, __LINE__));
}
void slim::builtins::require(const v8::FunctionCallbackInfo<v8::Value>& args) {
	log::trace(log::Message("slim::builtins::require","begins",__FILE__, __LINE__));
	v8::Isolate* isolate = args.GetIsolate();
	v8::Local<v8::Context> context = isolate->GetCurrentContext();
	v8::Local<v8::String> v8_plugin_name_string  = args[0]->ToString(context).ToLocalChecked();
	std::string plugin_name_string = slim::utilities::v8StringToString(isolate, v8_plugin_name_string);
	log::debug(log::Message("slim::builtins::require",std::string("plugin name => " +  plugin_name_string).c_str() ,__FILE__, __LINE__));
	slim::plugin::loader::load_plugin(isolate, plugin_name_string, true);
	v8::Local<v8::Value> v8_property_value = context->Global()->Get(context, v8_plugin_name_string).ToLocalChecked();
	if(!v8_property_value.IsEmpty() && v8_property_value->IsObject()) {
		v8::Local<v8::Object> target_object = v8::Local<v8::Object>::Cast(v8_property_value);
		args.GetReturnValue().Set(v8::Local<v8::Object>::Cast(v8_property_value));
		log::trace(log::Message("slim::builtins::require","found object",__FILE__, __LINE__));
	}
	log::trace(log::Message("slim::builtins::require","ends",__FILE__, __LINE__));
}