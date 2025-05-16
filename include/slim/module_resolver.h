#ifndef __SLIM__MODULE__RESOLVER__H
#define __SLIM__MODULE__RESOLVER__H
#include <filesystem>
#include <string>
#include <v8.h>
namespace slim::module::resolver {
	struct import_specifier {
		import_specifier();
		import_specifier(std::string specifier_string_in, v8::Local<v8::Context>& context, v8::Local<v8::Module> synthetic_module);
		import_specifier(std::string specifier_string_in, v8::Local<v8::Context>& context, const bool is_entry_point_value);
		v8::Local<v8::Module>& get_module();
		const std::string& get_module_status_string();
		const std::string& get_path() const;
		const std::string& get_specifier() const;
		const bool has_module() const;
		const bool is_entry_point() const;
		private:
			bool has_module_value = false;
			bool is_entry_point_value = false;
			v8::Isolate* isolate;
			v8::Local<v8::Context> context;
			v8::Local<v8::Module> v8_module;
			std::string v8_module_status;
			std::string specifier_string; // .js, .mjs, .ts file name
			std::string specifier_path_string;
			std::string specifier_source_code;
			std::filesystem::path specifier_path;
			void compile_module();
			void fetch_source();
			void instantiate_module();
	};
	using specifier_cache = std::unordered_map<std::string, import_specifier>;
	specifier_cache& resolve_imports(std::string entry_script_file_name_string_in, v8::Local<v8::Context> context, const bool is_entry_point_value);
	v8::MaybeLocal<v8::Module> module_call_back_resolver(v8::Local<v8::Context> context,
		v8::Local<v8::String> v8_input_file_name, v8::Local<v8::FixedArray> import_assertions, v8::Local<v8::Module> referrer);
}
#endif