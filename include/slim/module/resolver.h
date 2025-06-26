#ifndef __SLIM__MODULE__RESOLVER__H
#define __SLIM__MODULE__RESOLVER__H
#include <memory>
#include <string>
#include <unordered_map>
#include <v8.h>
#include <slim/module/import_specifier.h>
namespace slim::module::resolver {
	using specifier_cache_by_specifier = std::unordered_map<std::string, std::shared_ptr<import_specifier>>;
	using specifier_cache_by_hash_id = std::unordered_map<int, std::shared_ptr<import_specifier>>;
	std::shared_ptr<slim::module::import_specifier> resolve_imports(v8::Isolate* isolate,
		variant_specifier script_name_string_or_file_definition_struct, const bool is_entry_point_value);
	v8::MaybeLocal<v8::Module> module_call_back_resolver(v8::Local<v8::Context> context,
		v8::Local<v8::String> v8_input_file_name, v8::Local<v8::FixedArray> import_assertions, v8::Local<v8::Module> referrer);
	static void cache_import_specifier(std::shared_ptr<import_specifier> module_import_specifier);
	std::shared_ptr<slim::module::import_specifier> get_import_specifier_by_hash_id(int hash_id_int);
	std::shared_ptr<slim::module::import_specifier> get_import_specifier_by_specifier_string(std::string specificer_string);
}
#endif