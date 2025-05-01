#include <slim/gv8.h>
#include <slim/common/fetch.h>
#include <slim/common/log.h>
#include <slim/plugin.hpp>
/* #include <simdutf/simdutf.h>
#include <simdutf/simdutf.cpp> */
namespace slim::plugin::fs {
	void readFileUtf8(const v8::FunctionCallbackInfo<v8::Value>& args);
	namespace realpathSync {
		void native(const v8::FunctionCallbackInfo<v8::Value>& args);
	}
}
void slim::plugin::fs::readFileUtf8(const v8::FunctionCallbackInfo<v8::Value>& args) {
	using namespace slim::common::log;
	using namespace slim::utilities;
	trace(Message("slim::plugin::fs::readFileUtf8()", "begins", __FILE__, __LINE__));
	v8::Isolate* isolate = args.GetIsolate();
	if(!args.Length() > 0 && !args[0]->IsNull() && !args[0]->IsUndefined() && !args[0]->IsString()) {
		isolate->ThrowException(StringToV8String(isolate, "slim::plugin::fs::readFileUtf8() requires => file_name"));
	}
	const char* file_name = v8ValueToString(isolate, args[0]).c_str();
	using namespace slim::common::fetch;
	//simdutf::convert_latin1_to_utf8_safe(src, src_len, dst, dst_len);
	args.GetReturnValue().Set(StringToV8String(isolate, fetch(file_name).str()));
	trace(Message("slim::plugin::fs::readFileUtf8()", "ends", __FILE__, __LINE__));
}
void slim::plugin::fs::realpathSync::native(const v8::FunctionCallbackInfo<v8::Value>& args) {
	//v8::Isolate* isolate = args.GetIsolate();
	//slim::utilities::BoolToV8Boolean(isolate, false);
	//args.GetReturnValue().Set(slim::utilities::BoolToV8Boolean(isolate, false));
}
extern "C" void expose_plugin(v8::Isolate* isolate) {
	slim::plugin::plugin fs_plugin(isolate, "fs");
	fs_plugin.add_function("readFileUtf8", slim::plugin::fs::readFileUtf8);
	slim::plugin::plugin realpathSync_plugin(isolate, "realpathSync");
	realpathSync_plugin.add_function("native", slim::plugin::fs::realpathSync::native);
	fs_plugin.add_plugin("realpathSync", &realpathSync_plugin);
	fs_plugin.expose_plugin();
	return;
}
