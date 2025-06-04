#ifndef __SLIM__MODULE__IMPORT__SPECIFIER__H
#define __SLIM__MODULE__IMPORT__SPECIFIER__H
#include <filesystem>
#include <string>
#include <v8.h>
namespace slim::module {
	struct import_specifier {
		import_specifier();
		import_specifier(v8::Isolate* isolate, std::string specifier_string_in, v8::Local<v8::Module> synthetic_module);
		import_specifier(v8::Isolate* isolate, std::string specifier_string_in, const bool is_entry_point_value, v8::Local<v8::Module> referrer);
		void compile_module();
		void instantiate_module();
		int get_hash_id();
		v8::Local<v8::Module>& get_module();
		const std::string& get_module_status_string();
		const std::filesystem::path& get_specifier_path() const;
		const std::string& get_specifier() const;
		const std::string& get_source_code() const;
		const std::string& get_specifier_url() const;
		void set_source_code(std::string source_code);
		const bool has_module() const;
		const bool is_entry_point() const;
		private:
			bool has_module_value = false;
			bool is_entry_point_value = false;
			bool is_synthetic_module = false;
			bool is_typescript_compiler_initialized = false;
			v8::Isolate* isolate;
			v8::Local<v8::Context> context;
			v8::Local<v8::Module> v8_module;
			v8::Local<v8::Module> referrer;
			std::string v8_module_status;
			std::string specifier_string; // .js, .mjs, .ts file name
			std::string specifier_string_url;
			std::string compiled_specifier_string_url;
			std::string specifier_string_original;
			std::string specifier_source_code;
			std::string specifier_original_source_code;
			std::filesystem::path specifier_path;
			v8::Local<v8::Function> typescript_compile_function;
			void fetch_source();
			void resolve_module_path();
			void initialize_typescript_compiler();
			void call_typescript_compiler();
	};
}
#endif