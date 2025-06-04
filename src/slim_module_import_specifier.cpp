#include <set>
#include <unordered_set>
#include <vector>
#include <v8.h>
#include <slim/builtins/typescript.h>
#include <slim/common/log.h>
#include <slim/common/fetch_and_apply_macros.h>
#include <slim/exception_handler.h>
#include <slim/macros.h>
#include <slim/module/import_specifier.h>
#include <slim/module_resolver.h>
#include <slim/utilities.h>

namespace slim::module {
	using namespace slim;
	using namespace slim::common;
	std::string typescript_compiled_specifier_string_url;
	void receive_compiled_file_name(const v8::FunctionCallbackInfo<v8::Value>& args);
	std::array<char*, 3> file_extensions = {".mjs", ".ts", ".js"};
	std::vector<std::string> search_paths = {
		""
	};
}
slim::module::import_specifier::import_specifier() {}
slim::module::import_specifier::import_specifier(v8::Isolate* isolate, std::string specifier_string, 
		v8::Local<v8::Module> synthetic_module) : isolate(isolate), specifier_string(specifier_string), v8_module(synthetic_module) {
	log::trace(log::Message("slim::module::import_specifier::import_specifier()",std::string("begins => " + specifier_string).c_str(),__FILE__, __LINE__));
	context = isolate->GetCurrentContext();
	instantiate_module();
	v8_module->Evaluate(context).FromMaybe(v8::Local<v8::Value>());
	log::trace(log::Message("slim::module::import_specifier::import_specifier()",std::string("ends => " + specifier_string).c_str(),__FILE__, __LINE__));
}
slim::module::import_specifier::import_specifier(v8::Isolate* isolate, std::string specifier_string, const bool is_entry_point_value, v8::Local<v8::Module> referrer)
			: isolate(isolate), specifier_string(specifier_string), context(context), referrer(referrer), is_entry_point_value(is_entry_point_value) {
	log::trace(log::Message("slim::module::import_specifier::import_specifier()",std::string("begins => " + specifier_string).c_str(),__FILE__, __LINE__));
	context = isolate->GetCurrentContext();
	log::debug(log::Message("slim::module::import_specifier::import_specifier()","found isolate",__FILE__, __LINE__));
	specifier_string_original = specifier_string;
	log::debug(log::Message("slim::module::import_specifier::import_specifier()","looking for source_file_content_string_pointer",__FILE__, __LINE__));
	auto source_file_content_string_pointer = builtins::typescript::get_file_content_pointer(specifier_string);
	log::debug(log::Message("slim::module::import_specifier::import_specifier()","received source_file_content_string_pointer",__FILE__, __LINE__));
	if(!source_file_content_string_pointer->empty()) {
		specifier_source_code = macros::apply(source_file_content_string_pointer, specifier_string);
		log::debug(log::Message("slim::module::import_specifier::import_specifier()",std::string("received source for => " + specifier_string).c_str(),__FILE__, __LINE__));
	}
	else {
		resolve_module_path();
		fetch_source();
		memory_mapper::register_path("directories", specifier_string_url.substr(0, specifier_string_url.find_last_of("/")));
		builtins::typescript::put_file_content(specifier_string_url, specifier_source_code);
		if(!is_typescript_compiler_initialized) {
			initialize_typescript_compiler();
		}
		call_typescript_compiler();
		specifier_source_code = *memory_mapper::read("typescript_chain", typescript_compiled_specifier_string_url);
		specifier_string_url = typescript_compiled_specifier_string_url;
	}
	log::trace(log::Message("slim::module::import_specifier::import_specifier()",std::string("ends => " + this->get_specifier_url()).c_str(),__FILE__, __LINE__));
}
int slim::module::import_specifier::get_hash_id() {
	return v8_module->GetIdentityHash();
}
v8::Local<v8::Module>& slim::module::import_specifier::get_module() {
	return v8_module;
}
const std::string& slim::module::import_specifier::get_module_status_string() {
	switch(v8_module->GetStatus()) {
		case v8::Module::Status::kUninstantiated: v8_module_status = "v8::Module::Status::kUninstantiated"; break;
		case v8::Module::Status::kInstantiating: v8_module_status = "v8::Module::Status::kInstantiating"; break;
		case v8::Module::Status::kInstantiated: v8_module_status = "v8::Module::Status::kInstantiated"; break;
		case v8::Module::Status::kEvaluating: v8_module_status = "v8::Module::Status::kEvaluating"; break;
		case v8::Module::Status::kErrored: v8_module_status = "v8::Module::Status::kErrored"; break;
	}
	return v8_module_status;
}
const std::string& slim::module::import_specifier::get_specifier() const {
	return specifier_string;
}
const std::filesystem::path& slim::module::import_specifier::get_specifier_path() const {
	return specifier_path;
}
const std::string& slim::module::import_specifier::get_source_code() const {
	return specifier_source_code;
}
const std::string& slim::module::import_specifier::get_specifier_url() const {
	return specifier_string_url;
}
const bool slim::module::import_specifier::has_module() const {
	return has_module_value;
}
const bool slim::module::import_specifier::is_entry_point() const {
	return is_entry_point_value;
}
void slim::module::import_specifier::set_source_code(std::string source_code) {
	specifier_original_source_code = specifier_source_code;
	specifier_source_code = source_code;
}
void slim::module::import_specifier::compile_module() {
	log::trace(log::Message("slim::module::import_specifier::compile_module()",std::string("begins => " + specifier_string_url).c_str(),__FILE__, __LINE__));
	v8::TryCatch try_catch(isolate);
	v8::ScriptOrigin origin(utilities::StringToV8Value(isolate, specifier_path.string()), 0, 0, false, -1, utilities::StringToV8Value(isolate, ""), false, false, true);
	v8::ScriptCompiler::Source v8_module_source(utilities::StringToV8String(isolate, specifier_source_code), origin);
	v8::ScriptCompiler::CompileOptions module_compile_options(v8::ScriptCompiler::kProduceCompileHints);
	v8::ScriptCompiler::NoCacheReason module_no_cache_reason(v8::ScriptCompiler::kNoCacheNoReason);
	v8::MaybeLocal<v8::Module> temporary_module = v8::ScriptCompiler::CompileModule(isolate, &v8_module_source, module_compile_options, module_no_cache_reason);
	if(!temporary_module.IsEmpty()) {
		v8_module = temporary_module.ToLocalChecked();
		has_module_value = true;
		log::debug(log::Message("slim::module::import_specifier::compile_module()", std::string("v8::ScriptCompiler::CompileModule() hash id => " + std::to_string(v8_module->GetIdentityHash())).c_str() , __FILE__, __LINE__));
	}
	if(try_catch.HasCaught()) {
		log::error(log::Message("slim::module::import_specifier::compile_module()", "try_catch.HasCaught()",__FILE__, __LINE__));
		slim::exception_handler::v8_try_catch_handler(&try_catch);
	}
	log::trace(log::Message("slim::module::import_specifier::compile_module()",std::string("ends => " + specifier_string_url).c_str(),__FILE__, __LINE__));
}
void slim::module::import_specifier::fetch_source() {
	log::trace(log::Message("slim::module::import_specifier::fetch_source()",std::string("begins => " + specifier_string_url).c_str(),__FILE__, __LINE__));
	specifier_source_code = slim::common::fetch_and_apply_macros(specifier_string);
/* 	std::regex from_pattern("[[:space:]\n]+from[[:space:]\n]+[\\\"\\'][.](\\/.+[^\"])[\\\"\\']");
	std::string expanded_path_statement =  " from '" + specifier_path_string + "$1'";
	specifier_source_code = std::regex_replace(specifier_source_code, from_pattern, expanded_path_statement); */
	log::debug(log::Message("slim::module::import_specifier::fetch_source()",std::string("specifier_string => " + specifier_string_url).c_str(), __FILE__, __LINE__));
	//log::debug(log::Message("slim::module::import_specifier::fetch_source()",std::string("source_string => " + specifier_source_code).c_str(), __FILE__, __LINE__));
	log::trace(log::Message("slim::module::import_specifier::fetch_source()",std::string("ends => " + specifier_string_url).c_str(),__FILE__, __LINE__));
}
void slim::module::import_specifier::instantiate_module() {
	log::trace(log::Message("slim::module::import_specifier::instantiate_module()",std::string("begins => " + specifier_string).c_str(),__FILE__, __LINE__));
	v8::TryCatch try_catch(isolate);
	auto result = v8_module->InstantiateModule(context, slim::module::resolver::module_call_back_resolver);
	if(result.IsNothing()) {
		log::error(log::Message("slim::module::import_specifier::instantiate_module()",std::string("v8_module->InstantiateModule() produced nothing => " + specifier_string).c_str(), __FILE__, __LINE__));
	}
	if(try_catch.HasCaught()) {
		log::error(log::Message("slim::module::import_specifier::instantiate_module()", "try_catch.HasCaught()",__FILE__, __LINE__));
		slim::exception_handler::v8_try_catch_handler(&try_catch);
	}
	log::debug(log::Message("slim::module::import_specifier::instantiate_module()",
											std::string("v8_module->InstantiateModule() status => " + get_module_status_string()).c_str(),__FILE__, __LINE__));
	log::trace(log::Message("slim::module::import_specifier::instantiate_module()",std::string("ends => " + specifier_string).c_str(),__FILE__, __LINE__));
}
void slim::module::import_specifier::initialize_typescript_compiler() {
	log::trace(log::Message("slim::module::import_specifier::initialize_typescript_compiler()", std::string("begins => " + specifier_string_url).c_str(),__FILE__, __LINE__));
	int typescript_specifier_hash_id = builtins::typescript::get_specifier_module_hash_id();
	log::debug(log::Message("slim::module::import_specifier::initialize_typescript_compiler()", std::to_string(typescript_specifier_hash_id).c_str(),__FILE__, __LINE__));
	auto typescript_import_specifier = slim::module::resolver::get_import_specifier_by_hash_id(typescript_specifier_hash_id);
	log::debug(log::Message("slim::module::import_specifier::initialize_typescript_compiler()", "got import_specifier",__FILE__, __LINE__));
	if(!typescript_import_specifier->has_module()) {
		isolate->ThrowException(utilities::StringToV8Value(isolate, "slim::module::import_specifier::initialize_typescript_compiler() specifier not present"));
		return;
	}
	auto typescript_module_namespace_object = typescript_import_specifier->get_module()->GetModuleNamespace()->ToObject(context).ToLocalChecked();
	log::debug(log::Message("slim::module::import_specifier::initialize_typescript_compiler()", "got namespace object",__FILE__, __LINE__));
	auto v8_function_name_string = utilities::StringToV8String(isolate, "compile");
	v8::Local<v8::Value> v8_function_value;
	if(!typescript_module_namespace_object->Get(context, v8_function_name_string).ToLocal(&v8_function_value)) {
		log::error(log::Message("slim::module::import_specifier::initialize_typescript_compiler()", "Error getting function",__FILE__, __LINE__));
	}
	log::debug(log::Message("slim::module::import_specifier::initialize_typescript_compiler()", "got v8_function_value object",__FILE__, __LINE__));
	typescript_compile_function = v8::Local<v8::Function>::Cast(v8_function_value);
	log::debug(log::Message("slim::module::import_specifier::initialize_typescript_compiler()", "got typescript_compile_function",__FILE__, __LINE__));
	if(typescript_compile_function.IsEmpty()) {
		log::error(log::Message("slim::module::import_specifier::initialize_typescript_compiler()", "typescript_compile_function.IsEmpty()",__FILE__, __LINE__));
	}
	is_typescript_compiler_initialized = true;
	log::trace(log::Message("slim::module::import_specifier::initialize_typescript_compiler()", std::string("ends => " + specifier_string_url).c_str(),__FILE__, __LINE__));
}
void slim::module::receive_compiled_file_name(const v8::FunctionCallbackInfo<v8::Value>& args) {
	log::trace(log::Message("slim::module::receive_compiled_file_name()", "begins",__FILE__, __LINE__));
	typescript_compiled_specifier_string_url = utilities::v8ValueToString(args.GetIsolate(), args[0]);
	auto* isolate = args.GetIsolate();
	auto context = isolate->GetCurrentContext();
	auto messages_array = args[1].As<v8::Array>();
	for(int index = 0; index < messages_array->Length(); index++) {
		log::typescript_warning(log::Message("",
			utilities::v8ValueToString(isolate, messages_array->Get(context, index).ToLocalChecked()).c_str(), __FILE__, __LINE__));
	}
	log::trace(log::Message("slim::module::receive_compiled_file_name()", "ends",__FILE__, __LINE__));
}
void slim::module::import_specifier::call_typescript_compiler() {
	log::trace(log::Message("slim::module::import_specifier::call_typescript_compiler()", std::string("begins => " + specifier_string_url).c_str(),__FILE__, __LINE__));
	auto receive_compiled_file_name_function_template = v8::FunctionTemplate::New(isolate, receive_compiled_file_name);
	log::debug(log::Message("slim::module::run_typescript()", "created function template",__FILE__, __LINE__));
	auto receive_compiled_file_name_function_maybe = receive_compiled_file_name_function_template->GetFunction(context);
	if(receive_compiled_file_name_function_maybe.IsEmpty()) {
		isolate->ThrowException(utilities::StringToV8Value(isolate, "typescript compile function is null"));
		return;
	}
	auto receive_compiled_file_name_function = receive_compiled_file_name_function_maybe.ToLocalChecked();
	log::debug(log::Message("slim::module::run_typescript()", "created function",__FILE__, __LINE__));
    const int argc = 2;
    v8::Local<v8::Value> args[argc] = {utilities::StringToV8String(isolate, specifier_string_url),receive_compiled_file_name_function};
	log::debug(log::Message("slim::module::run_typescript()", "loaded function arguments array",__FILE__, __LINE__));
	auto call_result = typescript_compile_function->Call(context, context->Global(), argc, args).ToLocalChecked();
	log::debug(log::Message("slim::module::run_typescript()", "called function",__FILE__, __LINE__));
	if(call_result.IsEmpty()) {
		log::trace(log::Message("slim::module::import_specifier::call_typescript_compiler()", std::string("error calling function => " + specifier_string_url).c_str(),__FILE__, __LINE__));
	}
	log::trace(log::Message("slim::module::call_typescript_compiler()", std::string("ends => " + specifier_string_url).c_str(),__FILE__, __LINE__));
}
void slim::module::import_specifier::resolve_module_path() {
	log::trace(log::Message("slim::module::import_specifier::resolve_module_path()",std::string("begins => " + specifier_string).c_str(), __FILE__, __LINE__));
	v8::TryCatch try_catch(isolate);
	bool module_file_found = false;
	if(!specifier_string.starts_with("./") && !specifier_string.starts_with("/")) {
		log::debug(log::Message("slim::module::import_specifier::resolve_module_path()",std::string("specifier_string !starts with ./ or / => " + specifier_string).c_str(), __FILE__, __LINE__));
		for(auto& current_search_path : search_paths) {
			auto current_working_search_path = std::filesystem::absolute(current_search_path + specifier_string);
			log::debug(log::Message("slim::module::import_specifier::import_specifier()",current_working_search_path.string().c_str(),__FILE__, __LINE__));
			log::debug(log::Message("slim::module::import_specifier::import_specifier()",std::string("current_working_search_path.has_extension() => " 
																+ std::to_string(current_working_search_path.has_extension())).c_str(),__FILE__, __LINE__));
			if(current_working_search_path.has_extension()) {
				if(std::filesystem::exists(current_working_search_path)) {
					log::debug(log::Message("slim::module::import_specifier::import_specifier()",std::string("current_working_search_path exists() => " 
																						+ current_working_search_path.string()).c_str(),__FILE__, __LINE__));
					module_file_found = true;
					specifier_path = current_working_search_path;
					break;
				}
			}
			else {
				std::unordered_set<std::string> possible_module_names = {
					current_working_search_path.string(),
					current_working_search_path.string() + "/index"
				};
				for(auto& file_extension : file_extensions) {
					for(auto& possible_module_name : possible_module_names) {
						std::filesystem::path possible_module_file_path = possible_module_name + file_extension;
						if(std::filesystem::exists(possible_module_file_path)) {
							module_file_found = true;
							specifier_path = possible_module_file_path;
							break;
						}
						else {
							log::debug(log::Message("slim::module::import_specifier::import_specifier()",
								std::string("did not find => " + possible_module_file_path.string()).c_str(),__FILE__, __LINE__));
						}
					}
				}
			}
		}
	}
	else {
		log::debug(log::Message("slim::module::import_specifier::import_specifier()",std::string("specifier_string => " +  specifier_string).c_str(),__FILE__, __LINE__));
		log::debug(log::Message("slim::module::import_specifier::import_specifier()",std::string("referrer.IsEmpty() => " +  referrer.IsEmpty() ? "true" : "false").c_str(),__FILE__, __LINE__));

		
		if(referrer.IsEmpty()) {
			log::debug(log::Message("slim::module::import_specifier::import_specifier()",std::string("specifier_string => " +  specifier_string).c_str(),__FILE__, __LINE__));
			specifier_path = specifier_string.starts_with(".") ? std::filesystem::absolute(specifier_string.substr(2)) : std::filesystem::absolute(specifier_string);
		}
		else {
			auto referrer_import_specifier = slim::module::resolver::get_import_specifier_by_hash_id(referrer->GetIdentityHash());
			log::debug(log::Message("slim::module::import_specifier::import_specifier()",std::string("specifier_string => " +  specifier_string).c_str(),__FILE__, __LINE__));
			auto parent_path = referrer_import_specifier->get_specifier_path().parent_path().string();
			log::debug(log::Message("slim::module::import_specifier::import_specifier()",std::string("specifier_string => " +  specifier_string).c_str(),__FILE__, __LINE__));
			if(specifier_string.starts_with("./")) {
				log::debug(log::Message("slim::module::import_specifier::import_specifier()",std::string("specifier_string => " +  specifier_string).c_str(),__FILE__, __LINE__));
				specifier_path = std::filesystem::absolute(parent_path + "/" + specifier_string.substr(2));
				log::debug(log::Message("slim::module::import_specifier::import_specifier()",std::string("specifier_path.string() => " +  specifier_path.string()).c_str(),__FILE__, __LINE__));
			}
			log::debug(log::Message("slim::module::import_specifier::import_specifier()",std::string("specifier_string => " +  specifier_string).c_str(),__FILE__, __LINE__));
		}
		log::debug(log::Message("slim::module::import_specifier::import_specifier()",std::string("specifier_string => " +  specifier_string).c_str(),__FILE__, __LINE__));
		if(specifier_path.has_extension()) { 
			if(std::filesystem::exists(specifier_path)) {
				module_file_found = true;
			}
		}
		else {
			std::unordered_set<std::string> possible_module_names = {
				specifier_path.string(),
				specifier_path.string() + "/index"
			};
			for(auto& file_extension : file_extensions) {
				for(auto& possible_module_name : possible_module_names) {
					std::string module_file_path = possible_module_name + file_extension;
					if(std::filesystem::exists(module_file_path)) {
						module_file_found = true;
						specifier_path = module_file_path;
						break;
					}
				}
			}
		}
	}
	if(!module_file_found) {
		log::error(log::Message("slim::module::import_specifier::import_specifier()",std::string("module not found => " + specifier_string).c_str(),__FILE__, __LINE__));
		isolate->ThrowException(utilities::StringToV8String(isolate, "module not found => " + specifier_string));
	}
	else {
		specifier_string = specifier_path.string();
		specifier_string_url = "file://" + specifier_string;
	}
/* 	if(try_catch.HasCaught()) {
		error(Message("slim::module::import_specifier::import_specifier()",std::string("module not found => " + 
			v8ValueToString(isolate, try_catch.Exception())).c_str(),__FILE__, __LINE__));
		slim::exception_handler::v8_try_catch_handler(&try_catch);
	} */
	log::trace(log::Message("slim::module::import_specifier::resolve_module_path()",std::string("ends => " + specifier_string_url).c_str(), __FILE__, __LINE__));
}