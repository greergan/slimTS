/* #include <sys/types.h>

#include <fcntl.h>
#include <unistd.h>
 */
#include <sys/stat.h>
#include <cstdio> 

#include <fstream>
#include <string>
#include <unordered_map>
#include <slim/gv8.h>
#include <slim/common/fetch.h>
#include <slim/common/log.h>
#include <slim/plugin.hpp>
/* #include <simdutf/simdutf.h>
#include <simdutf/simdutf.cpp> */
namespace slim::plugin::fs {
	class SlimFileBuf;
	struct FileInfo;
	static std::unordered_map<std::string, FileInfo> open_files;
	void _close(const v8::FunctionCallbackInfo<v8::Value>& args);
	void fstat(const v8::FunctionCallbackInfo<v8::Value>& args);
	void _open(const v8::FunctionCallbackInfo<v8::Value>& args);
	void readFileUtf8(const v8::FunctionCallbackInfo<v8::Value>& args);
	namespace realpathSync {
		void native(const v8::FunctionCallbackInfo<v8::Value>& args);
	}
}
struct slim::plugin::fs::FileInfo {
	FILE* file_pointer = nullptr;
	int file_handle = -1;
	std::string file_name;
};
class slim::plugin::fs::SlimFileBuf : public std::filebuf {
	private:
		std::string _file_name_string;
		std::ios_base::openmode _mode;
	public:
		SlimFileBuf() {}
		explicit SlimFileBuf(const std::string& file_name_string, std::ios_base::openmode mode) : _file_name_string(file_name_string), _mode(mode) {
		}
		~SlimFileBuf() {
			if(is_open()) {
				close();
			}
		}
		inline int fd() {
			return _M_file.native_handle();
		}
		inline int open() {
			std::filebuf::open(_file_name_string, _mode);
			return fd();
		}
		inline int open(const std::string& file_name_string, std::ios_base::openmode mode) {
			_file_name_string = file_name_string;
			_mode = mode;
			return open();
		}
};
void slim::plugin::fs::_close(const v8::FunctionCallbackInfo<v8::Value>& args) {
	using namespace slim::common::log;
	using namespace slim::utilities;
	trace(Message("slim::plugin::fs::close()", "begins", __FILE__, __LINE__));
	v8::Isolate* isolate = args.GetIsolate();
	if(!args.Length() > 0 || !args[0]->IsNumber()) {
		isolate->ThrowException(StringToV8String(isolate, "slim::plugin::fs::close() requires => file_handle_as_int"));
		return;
	}
	int file_handle = V8ValueToInt(isolate, args[0]);
	for(auto& pair : open_files) {
		if(pair.second.file_handle == file_handle) {
			close(file_handle);
			fclose(pair.second.file_pointer);
			args.GetReturnValue().Set(v8::Boolean::New(isolate, true));
			return;
		}
	}
	args.GetReturnValue().SetUndefined();
	trace(Message("slim::plugin::fs::close()", "ends", __FILE__, __LINE__));
}
void slim::plugin::fs::fstat(const v8::FunctionCallbackInfo<v8::Value>& args) {
	using namespace slim::common::log;
	using namespace slim::utilities;
	trace(Message("slim::plugin::fs::fstat()", "begins", __FILE__, __LINE__));
	v8::Isolate* isolate = args.GetIsolate();
	if(!args.Length() > 0 || !args[0]->IsInt32()) {
		isolate->ThrowException(StringToV8String(isolate, "slim::plugin::fs::fstat() requires => file_handle_as_int"));
	}
	int file_handle = V8ValueToInt(isolate, args[0]);
	for(auto& pair : open_files) {
		if(pair.second.file_handle == file_handle) {
			struct stat file_stats;
			if(fstat(file_handle, &file_stats) == -1) {
				args.GetReturnValue().SetUndefined();
				return;
			}
			else {
				auto context = isolate->GetCurrentContext();
				auto file_stat_object = v8::Object::New(isolate);
				file_stat_object->DefineOwnProperty(context, StringToV8Name(isolate, "dev"), IntToV8Integer(isolate, file_stats.st_dev));
				file_stat_object->DefineOwnProperty(context, StringToV8Name(isolate, "ino"), IntToV8Integer(isolate, file_stats.st_ino));
				file_stat_object->DefineOwnProperty(context, StringToV8Name(isolate, "mode"), IntToV8Integer(isolate, file_stats.st_mode));
				file_stat_object->DefineOwnProperty(context, StringToV8Name(isolate, "nlink"), IntToV8Integer(isolate, file_stats.st_nlink));
				file_stat_object->DefineOwnProperty(context, StringToV8Name(isolate, "uid"), IntToV8Integer(isolate, file_stats.st_uid));
				file_stat_object->DefineOwnProperty(context, StringToV8Name(isolate, "gid"), IntToV8Integer(isolate, file_stats.st_gid));
				file_stat_object->DefineOwnProperty(context, StringToV8Name(isolate, "rdev"), IntToV8Integer(isolate, file_stats.st_rdev));
				file_stat_object->DefineOwnProperty(context, StringToV8Name(isolate, "size"), IntToV8Integer(isolate, file_stats.st_size));
				file_stat_object->DefineOwnProperty(context, StringToV8Name(isolate, "blksize"), IntToV8Integer(isolate, file_stats.st_blksize));
				file_stat_object->DefineOwnProperty(context, StringToV8Name(isolate, "blocks"), IntToV8Integer(isolate, file_stats.st_blocks));
				file_stat_object->DefineOwnProperty(context, StringToV8Name(isolate, "atimeMs"), DoubleToV8Number(isolate, time_spec_to_double(file_stats.st_atim)));
				file_stat_object->DefineOwnProperty(context, StringToV8Name(isolate, "mtimeMs"), DoubleToV8Number(isolate, time_spec_to_double(file_stats.st_mtim)));
				file_stat_object->DefineOwnProperty(context, StringToV8Name(isolate, "ctimeMs"), DoubleToV8Number(isolate, time_spec_to_double(file_stats.st_ctim)));
				auto atim_string = time_spec_to_time_string_gmt(file_stats.st_atim);
				file_stat_object->DefineOwnProperty(context, StringToV8Name(isolate, "atime"), StringToV8String(isolate, atim_string));
				auto mtim_string = time_spec_to_time_string_gmt(file_stats.st_mtim);
				file_stat_object->DefineOwnProperty(context, StringToV8Name(isolate, "mtime"), StringToV8String(isolate, mtim_string));
				auto ctim_string = time_spec_to_time_string_gmt(file_stats.st_ctim);
				file_stat_object->DefineOwnProperty(context, StringToV8Name(isolate, "ctime"), StringToV8String(isolate, ctim_string));
				//file_stats.st_attr <= DOS style
				//file_stats.st_birthtime <= Newer Linux kernels
				args.GetReturnValue().Set(file_stat_object);
				return;
			}
		}
	}
	trace(Message("slim::plugin::fs::fstat()", "end", __FILE__, __LINE__));
}
void slim::plugin::fs::_open(const v8::FunctionCallbackInfo<v8::Value>& args) {
	using namespace slim::common::log;
	using namespace slim::utilities;
	trace(Message("slim::plugin::fs::open()", "begins", __FILE__, __LINE__));
	v8::Isolate* isolate = args.GetIsolate();
	if(!args.Length() > 0) {
		debug(Message("slim::plugin::fs::open()", "problem with arguments", __FILE__, __LINE__));
		isolate->ThrowError(StringToV8String(isolate, "slim::plugin::fs::open() requires => file_name_as_string"));
		return;
	}
 	if(!args.Length() > 1) {
		debug(Message("slim::plugin::fs::open()", "problem with arguments", __FILE__, __LINE__));
		isolate->ThrowException(StringToV8String(isolate, "slim::plugin::fs::open() requires => mode_as_int_or_string"));
		return;
	}
	debug(Message("slim::plugin::fs::open()", "args are good", __FILE__, __LINE__));
	std::string file_name_string = v8ValueToString(isolate, args[0]);
	std::string mode_string;
	int mode_int = V8ValueToInt(isolate, args[1]);
	switch(mode_int) {
		case 0:
			mode_string = 'r';
			break;
		case 1:
			mode_string = 'w';
			break;
		default:
			throw("unhandled file mode");
	}
	//mode_string = v8ValueToString(isolate, args[1]);
	debug(Message("slim::plugin::fs::open()", "checking for file information", __FILE__, __LINE__));
	if(open_files[file_name_string].file_handle == -1) {
		debug(Message("slim::plugin::fs::open()", "file_handle starts as -1", __FILE__, __LINE__));
		open_files[file_name_string].file_name = file_name_string;
		debug(Message("slim::plugin::fs::open()", std::string("attempting to open file => " + file_name_string + " mode => " + mode_string).c_str(), __FILE__, __LINE__));
		open_files[file_name_string].file_pointer = fopen(file_name_string.c_str(), mode_string.c_str());
		if(open_files[file_name_string].file_pointer ==  NULL) {
			isolate->ThrowError(StringToV8String(isolate, std::string("unable to open file => " + open_files[file_name_string].file_name)));
		}
		else {
			open_files[file_name_string].file_handle = fileno(open_files[file_name_string].file_pointer);
			if(open_files[file_name_string].file_handle == -1) {
				fclose(open_files[file_name_string].file_pointer);
				isolate->ThrowError(StringToV8String(isolate, std::string("unable to get file_handle => " + open_files[file_name_string].file_name)));
			}
		}
	}
	args.GetReturnValue().Set(v8::Int32::New(isolate, open_files[file_name_string].file_handle));
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
	fs_plugin.add_function("close", slim::plugin::fs::_close);
	fs_plugin.add_function("closeSync", slim::plugin::fs::_close);
	fs_plugin.add_function("fstat", slim::plugin::fs::fstat);
	fs_plugin.add_function("open", slim::plugin::fs::_open);
	fs_plugin.add_function("readFileUtf8", slim::plugin::fs::readFileUtf8);
	slim::plugin::plugin realpathSync_plugin(isolate, "realpathSync");
	realpathSync_plugin.add_function("native", slim::plugin::fs::realpathSync::native);
	fs_plugin.add_plugin("realpathSync", &realpathSync_plugin);
	fs_plugin.expose_plugin();
	return;
}
