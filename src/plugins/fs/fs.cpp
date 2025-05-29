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
#include <slim/common/memory_mapper.h>
#include <slim/plugin.hpp>
/* #include <simdutf/simdutf.h>
#include <simdutf/simdutf.cpp> */
	using namespace slim::common;
	using namespace slim;
namespace slim::plugin::fs {
	class SlimFileBuf;
	struct FileInfo;
	static std::unordered_map<std::string, FileInfo> open_files;
	void _close(const v8::FunctionCallbackInfo<v8::Value>& args);
	void fstat(const v8::FunctionCallbackInfo<v8::Value>& args);
	void _open(const v8::FunctionCallbackInfo<v8::Value>& args);
	void readFileUtf8(const v8::FunctionCallbackInfo<v8::Value>& args);
	void stat(const v8::FunctionCallbackInfo<v8::Value>& args);;
	void memory_adaptor(const v8::FunctionCallbackInfo<v8::Value>& args);
	namespace realpathSync {
		void native(const v8::FunctionCallbackInfo<v8::Value>& args);
	}
	namespace memory_mapper {
		void read_file_sync(const v8::FunctionCallbackInfo<v8::Value>& args);
		void write_to_file_descriptor_sync(const v8::FunctionCallbackInfo<v8::Value>& args);
		void write_file_sync(const v8::FunctionCallbackInfo<v8::Value>& args);
		void stat_sync(const v8::FunctionCallbackInfo<v8::Value>& args);
	}
	void is_true(const v8::FunctionCallbackInfo<v8::Value>& args);
	void is_false(const v8::FunctionCallbackInfo<v8::Value>& args);
}
void ::slim::plugin::fs::is_true(const v8::FunctionCallbackInfo<v8::Value>& args) {
	args.GetReturnValue().Set(v8::Boolean::New(args.GetIsolate(), 1));
}
void ::slim::plugin::fs::is_false(const v8::FunctionCallbackInfo<v8::Value>& args) {
	args.GetReturnValue().Set(v8::Boolean::New(args.GetIsolate(), 0));
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
	log::trace(log::Message("slim::plugin::fs::close()", "begins", __FILE__, __LINE__));
	v8::Isolate* isolate = args.GetIsolate();
	if(!args.Length() > 0 || !args[0]->IsNumber()) {
		isolate->ThrowException(utilities::StringToV8String(isolate, "slim::plugin::fs::close() requires => file_handle_as_int"));
		return;
	}
	int file_handle = utilities::V8ValueToInt(isolate, args[0]);
	for(auto& pair : open_files) {
		if(pair.second.file_handle == file_handle) {
			close(file_handle);
			fclose(pair.second.file_pointer);
			args.GetReturnValue().Set(v8::Boolean::New(isolate, true));
			return;
		}
	}
	args.GetReturnValue().SetUndefined();
	log::trace(log::Message("slim::plugin::fs::close()", "ends", __FILE__, __LINE__));
}
void slim::plugin::fs::fstat(const v8::FunctionCallbackInfo<v8::Value>& args) {
	log::trace(log::Message("slim::plugin::fs::fstat()", "begins", __FILE__, __LINE__));
	v8::Isolate* isolate = args.GetIsolate();
	if(!args.Length() > 0 || !args[0]->IsInt32()) {
		isolate->ThrowException(utilities::StringToV8String(isolate, "slim::plugin::fs::fstat() requires => file_handle_as_int"));
	}
	int file_handle = utilities::V8ValueToInt(isolate, args[0]);
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
				using namespace slim::utilities;
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
	log::trace(log::Message("slim::plugin::fs::fstat()", "end", __FILE__, __LINE__));
}
void slim::plugin::fs::_open(const v8::FunctionCallbackInfo<v8::Value>& args) {
	log::trace(log::Message("slim::plugin::fs::open()", "begins", __FILE__, __LINE__));
	v8::Isolate* isolate = args.GetIsolate();
	if(!args.Length() > 0) {
		log::debug(log::Message("slim::plugin::fs::open()", "problem with arguments", __FILE__, __LINE__));
		isolate->ThrowError(utilities::StringToV8String(isolate, "slim::plugin::fs::open() requires => file_name_as_string"));
		return;
	}
 	if(!args.Length() > 1) {
		log::debug(log::Message("slim::plugin::fs::open()", "problem with arguments", __FILE__, __LINE__));
		isolate->ThrowException(utilities::StringToV8String(isolate, "slim::plugin::fs::open() requires => mode_as_int_or_string"));
		return;
	}
	log::debug(log::Message("slim::plugin::fs::open()", "args are good", __FILE__, __LINE__));
	std::string file_name_string = utilities::v8ValueToString(isolate, args[0]);
	std::string mode_string;
	int mode_int = utilities::V8ValueToInt(isolate, args[1]);
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
	log::debug(log::Message("slim::plugin::fs::open()", "checking for file information", __FILE__, __LINE__));
	if(open_files[file_name_string].file_handle == -1) {
		log::debug(log::Message("slim::plugin::fs::open()", "file_handle starts as -1", __FILE__, __LINE__));
		open_files[file_name_string].file_name = file_name_string;
		log::debug(log::Message("slim::plugin::fs::open()", std::string("attempting to open file => " + file_name_string + " mode => " + mode_string).c_str(), __FILE__, __LINE__));
		open_files[file_name_string].file_pointer = fopen(file_name_string.c_str(), mode_string.c_str());
		if(open_files[file_name_string].file_pointer ==  NULL) {
			isolate->ThrowError(utilities::StringToV8String(isolate, std::string("unable to open file => " + open_files[file_name_string].file_name)));
		}
		else {
			open_files[file_name_string].file_handle = fileno(open_files[file_name_string].file_pointer);
			if(open_files[file_name_string].file_handle == -1) {
				fclose(open_files[file_name_string].file_pointer);
				isolate->ThrowError(utilities::StringToV8String(isolate, std::string("unable to get file_handle => " + open_files[file_name_string].file_name)));
			}
		}
	}
	args.GetReturnValue().Set(v8::Int32::New(isolate, open_files[file_name_string].file_handle));
	log::trace(log::Message("slim::plugin::fs::open()", "ends", __FILE__, __LINE__));
}
void slim::plugin::fs::readFileUtf8(const v8::FunctionCallbackInfo<v8::Value>& args) {
	log::trace(log::Message("slim::plugin::fs::readFileUtf8()", "begins", __FILE__, __LINE__));
	v8::Isolate* isolate = args.GetIsolate();
	if(!args.Length() > 0 && !args[0]->IsNull() && !args[0]->IsUndefined() && !args[0]->IsString()) {
		isolate->ThrowException(utilities::StringToV8String(isolate, "slim::plugin::fs::readFileUtf8() requires => file_name"));
	}
	std::string file_name = utilities::v8ValueToString(isolate, args[0]);
	using namespace slim::common::fetch;
	//simdutf::convert_latin1_to_utf8_safe(src, src_len, dst, dst_len);
	args.GetReturnValue().Set(utilities::StringToV8String(isolate, fetch(file_name).str()));
	log::trace(log::Message("slim::plugin::fs::readFileUtf8()", "ends", __FILE__, __LINE__));
}
void slim::plugin::fs::realpathSync::native(const v8::FunctionCallbackInfo<v8::Value>& args) {
	//v8::Isolate* isolate = args.GetIsolate();
	//slim::utilities::BoolToV8Boolean(isolate, false);
	//args.GetReturnValue().Set(slim::utilities::BoolToV8Boolean(isolate, false));
}
void slim::plugin::fs::memory_mapper::read_file_sync(const v8::FunctionCallbackInfo<v8::Value>& args) {
	log::trace(log::Message("slim::plugin::fs::memory_mapper::read_file_sync()", "begins",__FILE__, __LINE__));
	auto* isolate = args.GetIsolate();
	if(args.Length() != 2) {
		isolate->ThrowError(utilities::StringToString(isolate, "fs.memoryAdaptor requires two string arguments"));
	}
/* 	if(args[0]->IsUndefined() || args[1]->IsUndefined() || !args[0]->IsString() || !args[1]->IsString()) {
		isolate->ThrowError(utilities::StringToString(isolate, "fs.memoryAdaptor requires two string argument"));
	} */
	//log::debug(log::Message("slim::plugin::fs::memory_mapper::read_file_sync()", "",__FILE__, __LINE__));
	auto chain_handle_string = utilities::v8ValueToString(isolate, args[0]);
	auto file_name_string = utilities::v8ValueToString(isolate, args[1]);
	//log::debug(log::Message("slim::plugin::fs::memory_mapper::read_file_sync()", chain_handle_string.c_str(),__FILE__, __LINE__));
	//log::debug(log::Message("slim::plugin::fs::memory_mapper::read_file_sync()", file_name_string.c_str(),__FILE__, __LINE__));
	auto file_content = slim::common::memory_mapper::read(chain_handle_string, file_name_string);
	//log::debug(log::Message("slim::plugin::fs::memory_mapper::read_file_sync()", std::string("file size => " + std::to_string(file_content.get()->size())).c_str(),__FILE__, __LINE__));
	args.GetReturnValue().Set(utilities::StringToV8String(isolate, *file_content.get()));
	log::trace(log::Message("slim::plugin::fs::memory_mapper::read_file_sync()", "ends",__FILE__, __LINE__));
}
void slim::plugin::fs::memory_mapper::stat_sync(const v8::FunctionCallbackInfo<v8::Value>& args) {
	log::trace(log::Message("slim::plugin::fs::memory_mapper::stat_sync()", "begins",__FILE__, __LINE__));
	auto* isolate = args.GetIsolate();
	auto context = isolate->GetCurrentContext();
	if(common::memory_mapper::exists(utilities::v8ValueToString(isolate, args[0]), utilities::v8ValueToString(isolate, args[1]))) {
		log::debug(log::Message("slim::plugin::fs::memory_mapper::stat_sync()", "a file exists",__FILE__, __LINE__));
		auto stats_object = v8::Object::New(isolate);
		auto is_true_function_template = v8::FunctionTemplate::New(isolate, is_true);
		auto is_true_function = is_true_function_template->GetFunction(context).ToLocalChecked();
		stats_object->Set(context, utilities::StringToV8String(isolate, "isFile"), is_true_function);
		auto is_false_function_template = v8::FunctionTemplate::New(isolate, is_false);
		auto is_false_function = is_false_function_template->GetFunction(context).ToLocalChecked();
		stats_object->Set(context, utilities::StringToV8String(isolate, "isDirectory"), is_false_function);
		args.GetReturnValue().Set(stats_object);
		log::debug(log::Message("slim::plugin::fs::memory_mapper::stat_sync()", "a file exists",__FILE__, __LINE__));
	}
	else if(common::memory_mapper::exists("directories", utilities::v8ValueToString(isolate, args[1]))) {
		log::debug(log::Message("slim::plugin::fs::memory_mapper::stat_sync()", "a directory exists",__FILE__, __LINE__));
		auto stats_object = v8::Object::New(isolate);
		auto is_false_function_template = v8::FunctionTemplate::New(isolate, is_false);
		auto is_false_function = is_false_function_template->GetFunction(context).ToLocalChecked();
		stats_object->Set(context, utilities::StringToV8String(isolate, "isFile"), is_false_function);
		auto is_true_function_template = v8::FunctionTemplate::New(isolate, is_true);
		auto is_true_function = is_true_function_template->GetFunction(context).ToLocalChecked();
		stats_object->Set(context, utilities::StringToV8String(isolate, "isDirectory"), is_true_function);
		args.GetReturnValue().Set(stats_object);
		log::debug(log::Message("slim::plugin::fs::memory_mapper::stat_sync()", "a directory exists",__FILE__, __LINE__));
	}
	else {
		args.GetReturnValue().SetUndefined();
		log::debug(log::Message("slim::plugin::fs::memory_mapper::stat_sync()", "found nothing return => undefined",__FILE__, __LINE__));
	}
	log::trace(log::Message("slim::plugin::fs::memory_mapper::stat_sync()", "ends",__FILE__, __LINE__));
}
void slim::plugin::fs::memory_mapper::write_file_sync(const v8::FunctionCallbackInfo<v8::Value>& args) {
	log::trace(log::Message("slim::plugin::fs::memory_mapper::write_file_sync()", "begins",__FILE__, __LINE__));
	auto* isolate = args.GetIsolate();
	auto chain_handle_string = utilities::v8ValueToString(isolate, args[0]);
	auto file_name_string = utilities::v8ValueToString(isolate, args[1]);
	auto file_data_string = utilities::v8ValueToString(isolate, args[2]);
	log::debug(log::Message("slim::plugin::fs::memory_mapper::write_file_sync()", chain_handle_string.c_str(),__FILE__, __LINE__));
	log::debug(log::Message("slim::plugin::fs::memory_mapper::write_file_sync()", file_name_string.c_str(),__FILE__, __LINE__));
	slim::common::memory_mapper::write(chain_handle_string, file_name_string, file_data_string);
	log::trace(log::Message("slim::plugin::fs::memory_mapper::write_file_sync()", "ends",__FILE__, __LINE__));
}
void slim::plugin::fs::memory_mapper::write_to_file_descriptor_sync(const v8::FunctionCallbackInfo<v8::Value>& args) {
	log::trace(log::Message("slim::plugin::fs::memory_mapper::write_to_file_descriptor_sync()", "begins",__FILE__, __LINE__));
	auto* isolate = args.GetIsolate();
	auto chain_handle_string = utilities::v8ValueToString(isolate, args[0]);
	auto file_name_string = utilities::v8ValueToString(isolate, args[1]);
	auto file_data_string = utilities::v8ValueToString(isolate, args[2]);
	log::debug(log::Message("slim::plugin::fs::memory_mapper::write_to_file_descriptor_sync()", chain_handle_string.c_str(),__FILE__, __LINE__));
	log::debug(log::Message("slim::plugin::fs::memory_mapper::write_to_file_descriptor_sync()", file_name_string.c_str(),__FILE__, __LINE__));
	slim::common::memory_mapper::write(chain_handle_string, file_name_string, file_data_string);
	log::trace(log::Message("slim::plugin::fs::memory_mapper::write_to_file_descriptor_sync()", "begins",__FILE__, __LINE__));
}
void slim::plugin::fs::memory_adaptor(const v8::FunctionCallbackInfo<v8::Value>& args) {
	log::trace(log::Message("slim::plugin::fs::memory_adaptor()", "begins",__FILE__, __LINE__));
	v8::Isolate* isolate = args.GetIsolate();
/* 	if(args[0]->IsUndefined() || !args[0]->IsString()) {
		isolate->ThrowError(utilities::StringToString(isolate, "fs.memoryAdaptor requires a single string argument"));
		log::error(log::Message("slim::plugin::fs::memory_adaptor()", "error",__FILE__, __LINE__));
	} */
	log::debug(log::Message("slim::plugin::fs::memory_adaptor()", "",__FILE__, __LINE__));
	auto context = isolate->GetCurrentContext();
	auto read_file_function_template = v8::FunctionTemplate::New(isolate, memory_mapper::read_file_sync);
	auto write_file_function_template = v8::FunctionTemplate::New(isolate, memory_mapper::write_file_sync);
	auto write_file_descriptor_function_template = v8::FunctionTemplate::New(isolate, memory_mapper::write_to_file_descriptor_sync);
	auto stat_sync_function_template = v8::FunctionTemplate::New(isolate, memory_mapper::stat_sync);
	log::debug(log::Message("slim::plugin::fs::memory_adaptor()", "function templates created",__FILE__, __LINE__));
	auto read_file_function = read_file_function_template->GetFunction(context).ToLocalChecked();
	auto write_file_function = write_file_function_template->GetFunction(context).ToLocalChecked();
	auto write_file_descriptor_function = write_file_descriptor_function_template->GetFunction(context).ToLocalChecked();
	auto stat_sync_function = stat_sync_function_template->GetFunction(context).ToLocalChecked();
	log::debug(log::Message("slim::plugin::fs::memory_adaptor()", "functions created",__FILE__, __LINE__));
	auto memory_adaptor_object = v8::Object::New(isolate);
	log::debug(log::Message("slim::plugin::fs::memory_adaptor()", "created memory_adaptor_object",__FILE__, __LINE__));
	memory_adaptor_object->Set(context, utilities::StringToV8String(isolate, "readFileSync"), read_file_function);
	memory_adaptor_object->Set(context, utilities::StringToV8String(isolate, "writeSync"), write_file_descriptor_function);
	memory_adaptor_object->Set(context, utilities::StringToV8String(isolate, "writeFileSync"), write_file_function);
	memory_adaptor_object->Set(context, utilities::StringToV8String(isolate, "statSync"), stat_sync_function);
	log::debug(log::Message("slim::plugin::fs::memory_adaptor()", "set memory_adaptor_object functions",__FILE__, __LINE__));
	args.GetReturnValue().Set(memory_adaptor_object);
	log::trace(log::Message("slim::plugin::fs::memory_adaptor()", "ends",__FILE__, __LINE__));
}
extern "C" void expose_plugin(v8::Isolate* isolate) {
	slim::plugin::plugin fs_plugin(isolate, "fs");
	fs_plugin.add_function("close", slim::plugin::fs::_close);
	fs_plugin.add_function("closeSync", slim::plugin::fs::_close);
	fs_plugin.add_function("fstat", slim::plugin::fs::fstat);
	fs_plugin.add_function("memoryAdaptor", slim::plugin::fs::memory_adaptor);
	fs_plugin.add_function("open", slim::plugin::fs::_open);
	fs_plugin.add_function("readFileUtf8", slim::plugin::fs::readFileUtf8);
	slim::plugin::plugin realpathSync_plugin(isolate, "realpathSync");
	realpathSync_plugin.add_function("native", slim::plugin::fs::realpathSync::native);
	fs_plugin.add_plugin("realpathSync", &realpathSync_plugin);
	fs_plugin.expose_plugin();
	return;
}
