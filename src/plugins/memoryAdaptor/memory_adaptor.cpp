#include <string>
#include <slim/common/log.h>
#include <slim/common/memory_mapper.h>
#include <slim/plugin.hpp>
namespace slim::plugin::memory_adaptor {
	using namespace slim::common;
	using namespace slim;
	void attach(const v8::FunctionCallbackInfo<v8::Value>& args);
	void exists(const v8::FunctionCallbackInfo<v8::Value>& args);
	void read(const v8::FunctionCallbackInfo<v8::Value>& args);
	void write(const v8::FunctionCallbackInfo<v8::Value>& args);
	void is_true(const v8::FunctionCallbackInfo<v8::Value>& args);
	void is_false(const v8::FunctionCallbackInfo<v8::Value>& args);
}
void ::slim::plugin::memory_adaptor::is_true(const v8::FunctionCallbackInfo<v8::Value>& args) {
	args.GetReturnValue().Set(v8::Boolean::New(args.GetIsolate(), true));
}
void ::slim::plugin::memory_adaptor::is_false(const v8::FunctionCallbackInfo<v8::Value>& args) {
	args.GetReturnValue().Set(v8::Boolean::New(args.GetIsolate(), false));
}
void slim::plugin::memory_adaptor::attach(const v8::FunctionCallbackInfo<v8::Value>& args) {
	log::trace(log::Message("slim::plugin::memory_adaptor::attach()", "begins",__FILE__, __LINE__));
	auto* isolate = args.GetIsolate();
	auto chain_handle_string = utilities::v8ValueToString(isolate, args[0]);
	log::debug(log::Message("slim::plugin::memory_adaptor::attach()", std::string("begins =>" + chain_handle_string).c_str(),__FILE__, __LINE__));
	memory_mapper::attach(chain_handle_string);
	log::trace(log::Message("slim::plugin::memory_adaptor::attach()", "ends",__FILE__, __LINE__));
}
void slim::plugin::memory_adaptor::exists(const v8::FunctionCallbackInfo<v8::Value>& args) {
	log::trace(log::Message("slim::plugin::memory_adaptor::exists()", "begins",__FILE__, __LINE__));
	auto* isolate = args.GetIsolate();
	auto context = isolate->GetCurrentContext();
	if(memory_mapper::exists(utilities::v8ValueToString(isolate, args[0]), utilities::v8ValueToString(isolate, args[1]))) {
		log::debug(log::Message("slim::plugin::memory_adaptor::exists()", "begins => content exists",__FILE__, __LINE__));
		auto stats_object = v8::Object::New(isolate);
		auto is_true_function_template = v8::FunctionTemplate::New(isolate, is_true);
		auto is_true_function = is_true_function_template->GetFunction(context).ToLocalChecked();
		stats_object->Set(context, utilities::StringToV8String(isolate, "isContent"), is_true_function);
		auto is_false_function_template = v8::FunctionTemplate::New(isolate, is_false);
		auto is_false_function = is_false_function_template->GetFunction(context).ToLocalChecked();
		stats_object->Set(context, utilities::StringToV8String(isolate, "isDirectory"), is_false_function);
		args.GetReturnValue().Set(stats_object);
		log::debug(log::Message("slim::plugin::memory_adaptor::exists()", "ends => content exists",__FILE__, __LINE__));
	}
	else if(memory_mapper::exists("directories", utilities::v8ValueToString(isolate, args[1]))) {
		log::debug(log::Message("slim::plugin::memory_adaptor::exists()", "begins => directory exists",__FILE__, __LINE__));
		auto stats_object = v8::Object::New(isolate);
		auto is_false_function_template = v8::FunctionTemplate::New(isolate, is_false);
		auto is_false_function = is_false_function_template->GetFunction(context).ToLocalChecked();
		stats_object->Set(context, utilities::StringToV8String(isolate, "isContent"), is_false_function);
		auto is_true_function_template = v8::FunctionTemplate::New(isolate, is_true);
		auto is_true_function = is_true_function_template->GetFunction(context).ToLocalChecked();
		stats_object->Set(context, utilities::StringToV8String(isolate, "isDirectory"), is_true_function);
		args.GetReturnValue().Set(stats_object);
		log::debug(log::Message("slim::plugin::memory_adaptor::exists()", "ends => directory exists",__FILE__, __LINE__));
	}
	else {
		args.GetReturnValue().SetUndefined();
		log::debug(log::Message("slim::plugin::memory_adaptor::exists()", "found nothing return => undefined",__FILE__, __LINE__));
	}
	log::trace(log::Message("slim::plugin::memory_adaptor::exists()", "ends",__FILE__, __LINE__));
}
void slim::plugin::memory_adaptor::read(const v8::FunctionCallbackInfo<v8::Value>& args) {
	log::trace(log::Message("slim::plugin::memory_adaptor::read()", "begins",__FILE__, __LINE__));
	auto* isolate = args.GetIsolate();
	if(args.Length() != 2) {
		isolate->ThrowError(utilities::StringToString(isolate, "memory_adaptor.memoryAdaptor requires two string arguments"));
	}
/* 	if(args[0]->IsUndefined() || args[1]->IsUndefined() || !args[0]->IsString() || !args[1]->IsString()) {
		isolate->ThrowError(utilities::StringToString(isolate, "memory_adaptor.memoryAdaptor requires two string argument"));
	} */
	//log::debug(log::Message("slim::plugin::memory_adaptor::read()", "",__FILE__, __LINE__));
	auto chain_handle_string = utilities::v8ValueToString(isolate, args[0]);
	auto content_name_string = utilities::v8ValueToString(isolate, args[1]);
	log::debug(log::Message("slim::plugin::memory_adaptor::read()", chain_handle_string.c_str(),__FILE__, __LINE__));
	log::debug(log::Message("slim::plugin::memory_adaptor::read()", content_name_string.c_str(),__FILE__, __LINE__));
	auto content_string_pointer = memory_mapper::read(chain_handle_string, content_name_string);
	log::debug(log::Message("slim::plugin::memory_adaptor::read()", std::string("content size => " + std::to_string(content_string_pointer.get()->size())).c_str(),__FILE__, __LINE__));
	args.GetReturnValue().Set(utilities::StringToV8String(isolate, *content_string_pointer.get()));
	log::trace(log::Message("slim::plugin::memory_adaptor::read()", "ends",__FILE__, __LINE__));
}
void slim::plugin::memory_adaptor::write(const v8::FunctionCallbackInfo<v8::Value>& args) {
	log::trace(log::Message("slim::plugin::memory_adaptor::write()", "begins",__FILE__, __LINE__));
	auto* isolate = args.GetIsolate();
	auto chain_handle_string = utilities::v8ValueToString(isolate, args[0]);
	auto content_name_string = utilities::v8ValueToString(isolate, args[1]);
	auto content_data_string = utilities::v8ValueToString(isolate, args[2]);
	log::debug(log::Message("slim::plugin::memory_adaptor::write()", chain_handle_string.c_str(),__FILE__, __LINE__));
	log::debug(log::Message("slim::plugin::memory_adaptor::write()", content_name_string.c_str(),__FILE__, __LINE__));
	log::debug(log::Message("slim::plugin::memory_adaptor::write()", std::to_string(content_data_string.length()).c_str(),__FILE__, __LINE__));
	memory_mapper::write(chain_handle_string, content_name_string, content_data_string);
	log::trace(log::Message("slim::plugin::memory_adaptor::write()", "ends",__FILE__, __LINE__));
}
extern "C" void expose_plugin(v8::Isolate* isolate) {
	slim::plugin::plugin memory_adaptor_plugin(isolate, "memoryAdaptor");
	memory_adaptor_plugin.add_function("attach", slim::plugin::memory_adaptor::attach);
	memory_adaptor_plugin.add_function("exists", slim::plugin::memory_adaptor::exists);
	memory_adaptor_plugin.add_function("read", slim::plugin::memory_adaptor::read);
	memory_adaptor_plugin.add_function("write", slim::plugin::memory_adaptor::write);
	memory_adaptor_plugin.expose_plugin();
	return;
}
