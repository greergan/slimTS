#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <v8.h>
#include <slim/common/log.h>
#include <slim/utilities.h>
namespace slim::plugin::fetch {
	using namespace slim;
	using namespace slim::common;
	void fetch(const v8::FunctionCallbackInfo<v8::Value>& args);
}
void slim::plugin::fetch::fetch(const v8::FunctionCallbackInfo<v8::Value>& args) {
	auto* isolate = args.GetIsolate();
	v8::HandleScope isolate_scope(isolate);
	auto context = isolate->GetCurrentContext();
	auto file_to_fetch_string = utilities::v8ValueToString(isolate, args[0]);
	log::trace(log::Message("slim::plugin::fetch::fetch()", "begins, file name => " + file_to_fetch_string,__FILE__,__LINE__));
	long file_size = 0;
	auto v8_response_object = v8::Object::New(isolate);
	args.GetReturnValue().Set(v8_response_object);
	log::trace(log::Message("slim::plugin::fetch::fetch()", "ends, file name => " + file_to_fetch_string + ", file size => " + std::to_string(file_size),__FILE__,__LINE__));
}
extern "C" void expose_plugin(v8::Isolate* isolate) {
	using namespace slim::common;
	v8::HandleScope isolate_scope(isolate);
	auto context = isolate->GetCurrentContext();
	log::trace(log::Message("slim::plugin::fetch::expose_plugin()", "begins",__FILE__,__LINE__));
	auto fetch_function_template = v8::FunctionTemplate::New(isolate, slim::plugin::fetch::fetch);
	auto fetch_function = fetch_function_template->GetFunction(context).ToLocalChecked();
	context->Global()->Set(context, slim::utilities::StringToV8Name(isolate, "fetch"), fetch_function).ToChecked();
	log::trace(log::Message("slim::plugin::fetch::expose_plugin()", "ends",__FILE__,__LINE__));
}
