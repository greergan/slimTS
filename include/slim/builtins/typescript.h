#ifndef __SLIM__BUILTINS__TYPESCRIPT__H
#define __SLIM__BUILTINS__TYPESCRIPT__H
#include <memory>
#include <string>
#include <unordered_map>
#include <v8.h>
#include <slim/common/log.h>
#include <slim/utilities.h>
namespace slim::builtins::typescript {
	extern std::unordered_map<std::string, std::shared_ptr<std::string>> raw_typescript_pipe_files;
	std::shared_ptr<std::string> get_file_content_pointer(std::string file_name_string);
	int get_specifier_module_hash_id();
	void initialize(v8::Isolate* isolate);
}
#endif