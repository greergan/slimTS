#ifndef __SLIM__BUILTINS__TYPESCRIPT__H
#define __SLIM__BUILTINS__TYPESCRIPT__H
#include <unordered_map>
#include <v8.h>
#include <slim/common/log.h>
#include <slim/utilities.h>
namespace slim::builtins::typescript {
	extern std::unordered_map<std::string, std::string> raw_typescript_definitions;
	void initialize(v8::Isolate* isolate);
	int get_specifier_module_hash_id();
}
#endif