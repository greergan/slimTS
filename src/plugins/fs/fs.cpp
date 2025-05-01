#include <fstream>
#include <vector>
#include <slim/gv8.h>
#include <slim/common/fetch.h>
#include <slim/common/log.h>
#include <slim/plugin.hpp>
/* #include <simdutf/simdutf.h>
#include <simdutf/simdutf.cpp> */
namespace slim::plugin::fs {
	static std::vector<std::fstream> possible_read_possible_write_fstreams;
	void close(const v8::FunctionCallbackInfo<v8::Value>& args);
	void fstat(const v8::FunctionCallbackInfo<v8::Value>& args);
	void open(const v8::FunctionCallbackInfo<v8::Value>& args);
	void readFileUtf8(const v8::FunctionCallbackInfo<v8::Value>& args);
	namespace realpathSync {
		void native(const v8::FunctionCallbackInfo<v8::Value>& args);
	}
}
void slim::plugin::fs::close(const v8::FunctionCallbackInfo<v8::Value>& args) {
	using namespace slim::common::log;
	using namespace slim::utilities;
	trace(Message("slim::plugin::fs::close()", "begins", __FILE__, __LINE__));
	v8::Isolate* isolate = args.GetIsolate();
	if(!args.Length() > 0 && !args[0]->IsNull() && !args[0]->IsUndefined() && !args[0]->IsInt32()) {
		isolate->ThrowException(StringToV8String(isolate, "slim::plugin::fs::close() requires => file_handle_as_int"));
	}
	int file_handle = V8ValueToInt(isolate, args[0]);
	possible_read_possible_write_fstreams[file_handle].close();
	args.GetReturnValue().SetUndefined();
	trace(Message("slim::plugin::fs::close()", "ends", __FILE__, __LINE__));
}
void slim::plugin::fs::fstat() {
	using namespace slim::common::log;
	using namespace slim::utilities;
	trace(Message("slim::plugin::fs::fstat()", "begins", __FILE__, __LINE__));
/* 	struct stat file_stat;
    if (fstat(fd, &file_stat) == -1) {
        std::cerr << "Error getting file status" << std::endl;
        return 1;
    } */
	trace(Message("slim::plugin::fs::fstat()", "begins", __FILE__, __LINE__));
}
void slim::plugin::fs::open(const v8::FunctionCallbackInfo<v8::Value>& args) {
	using namespace slim::common::log;
	using namespace slim::utilities;
	trace(Message("slim::plugin::fs::open()", "begins", __FILE__, __LINE__));
	v8::Isolate* isolate = args.GetIsolate();
	if(!args.Length() > 0 && !args[0]->IsNull() && !args[0]->IsUndefined() && !args[0]->IsString()) {
		isolate->ThrowException(StringToV8String(isolate, "slim::plugin::fs::open() requires => file_name"));
	}
	trace(Message("slim::plugin::fs::open()", "begins", __FILE__, __LINE__));
	std::string file_name = v8ValueToString(isolate, args[0]);
	trace(Message("slim::plugin::fs::open()", "begins", __FILE__, __LINE__));
	bool found_usable_stream = false;
	int stream_file_handle = 0;
	int streams_counter = 0;
	for(auto& stream : possible_read_possible_write_fstreams) {
		if(!stream.is_open()) {
			found_usable_stream = true;
			stream_file_handle = streams_counter;
			stream.open(file_name, std::ios::in);
			int fd = file.rdbuf()->fd();
			break;
		}
		streams_counter++;
	}
	if(!found_usable_stream) {
		trace(Message("slim::plugin::fs::open()", "!found_usable_stream", __FILE__, __LINE__));
		possible_read_possible_write_fstreams.push_back(std::fstream());
		stream_file_handle = possible_read_possible_write_fstreams.size() - 1;
		trace(Message("slim::plugin::fs::open()", std::string("past stream_file_handle => " + std::to_string(stream_file_handle)).c_str(), __FILE__, __LINE__));
		possible_read_possible_write_fstreams[stream_file_handle].open(file_name, std::ios::in);
		trace(Message("slim::plugin::fs::open()", "past stream_file_handle.open() ", __FILE__, __LINE__));
	}
	args.GetReturnValue().Set(v8::Integer::New(isolate, stream_file_handle));
	trace(Message("slim::plugin::fs::open()", "ends", __FILE__, __LINE__));
}
void slim::plugin::fs::readFileUtf8(const v8::FunctionCallbackInfo<v8::Value>& args) {
	using namespace slim::common::log;
	using namespace slim::utilities;
	trace(Message("slim::plugin::fs::readFileUtf8()", "begins", __FILE__, __LINE__));
	v8::Isolate* isolate = args.GetIsolate();
	if(!args.Length() > 0 && !args[0]->IsNull() && !args[0]->IsUndefined() && !args[0]->IsString()) {
		isolate->ThrowException(StringToV8String(isolate, "slim::plugin::fs::readFileUtf8() requires => file_name"));
	}
	std::string file_name = v8ValueToString(isolate, args[0]);
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
	fs_plugin.add_function("close", slim::plugin::fs::close);
	fs_plugin.add_function("fstat", slim::plugin::fs::fstat);
	fs_plugin.add_function("open", slim::plugin::fs::open);
	fs_plugin.add_function("readFileUtf8", slim::plugin::fs::readFileUtf8);
	slim::plugin::plugin realpathSync_plugin(isolate, "realpathSync");
	realpathSync_plugin.add_function("native", slim::plugin::fs::realpathSync::native);
	fs_plugin.add_plugin("realpathSync", &realpathSync_plugin);
	fs_plugin.expose_plugin();
	return;
}
