#include <future>
#include <set>
#include <unordered_set>
#include <vector>
#include <v8.h>
#include <slim/common/fetch.h>
#include <slim/common/log.h>
#include <slim/common/memory_mapper.h>
#include <slim/exception_handler.h>
#include <slim/macros.h>
#include <slim/module/import_specifier.h>
#include <slim/module/resolver.h>
#include <slim/queue/queue.h>
#include <slim/utilities.h>
namespace slim::module {
	using namespace slim;
	using namespace slim::common;
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
slim::module::import_specifier::import_specifier(v8::Isolate* isolate,  variant_specifier script_name_string_or_file_definition_struct,
		const bool is_entry_point_value, v8::Local<v8::Module> referrer)
			: isolate(isolate), referrer(referrer), is_entry_point_value(is_entry_point_value) {
	log::trace(log::Message("slim::module::import_specifier::import_specifier()","begins",__FILE__, __LINE__));
	context = isolate->GetCurrentContext();
	std::shared_ptr<slim::module::import_specifier> module_import_specifier_pointer;
	// files stored in compiled "library files" such as javascript servers i.e. typescript, less etc...
	if(std::holds_alternative<specifier_definition>(script_name_string_or_file_definition_struct)) {
		auto specifier_definition_stuct = std::get<specifier_definition>(script_name_string_or_file_definition_struct);
		log::trace(log::Message("slim::module::import_specifier::import_specifier()",std::string("preparing new specifier from stored file => " + specifier_definition_stuct.specifier_string_url).c_str(),__FILE__, __LINE__));
		specifier_string = specifier_definition_stuct.specifier_string_url;
		specifier_string_url = specifier_definition_stuct.specifier_string_url;
		specifier_original_source_code_pointer = specifier_definition_stuct.specifier_file_pointer;
		specifier_source_code_pointer = macros::apply(specifier_original_source_code_pointer, specifier_string_url);
	}
	else {
		specifier_string = std::get<std::string>(script_name_string_or_file_definition_struct);
		log::trace(log::Message("slim::module::import_specifier::import_specifier()",std::string("preparing new specifier from disk file => " + specifier_string).c_str(),__FILE__, __LINE__));
		specifier_string_original = specifier_string;
		resolve_module_path();
		fetch_source();
		memory_mapper::write("source_code_storage", specifier_string_url, specifier_source_code_pointer);
		std::string queue_name_string("typescript");
		slim::queue::job* transpile_typescript_job = new slim::queue::job(queue_name_string, "source_code_storage", specifier_string_url);
		transpile_typescript_job->egress_job_file.storage_container_handle = "typescript_storage";
		std::async(std::launch::async, slim::queue::submit, transpile_typescript_job);
		if(transpile_typescript_job->errored) {
			log::debug(log::Message("slim::module::import_specifier::import_specifier()","job completed with errors",__FILE__, __LINE__));
			std::string errors;
			for(auto&& error : transpile_typescript_job->errors) {
				errors += error + "\n";
			}
			delete transpile_typescript_job;
			log::debug(log::Message("slim::module::import_specifier::import_specifier()",errors.c_str(),__FILE__, __LINE__));
			isolate->ThrowException(utilities::StringToV8String(isolate, errors));
		}
		else {
			slim::queue::file_storage& egress_job_file = transpile_typescript_job->egress_job_file;
			specifier_source_code_pointer = memory_mapper::read(egress_job_file.storage_container_handle, egress_job_file.file_name_string);
			log::debug(log::Message("slim::module::import_specifier::import_specifier()",egress_job_file.storage_container_handle.c_str(),__FILE__, __LINE__));
			log::debug(log::Message("slim::module::import_specifier::import_specifier()",egress_job_file.file_name_string.c_str(),__FILE__, __LINE__));
			log::debug(log::Message("slim::module::import_specifier::import_specifier()",std::string("file size => " + std::to_string(specifier_source_code_pointer.get()->length())).c_str(),__FILE__, __LINE__));
			delete transpile_typescript_job;
		}
	}
	log::trace(log::Message("slim::module::import_specifier::import_specifier()",std::string("ends => " + specifier_string_url).c_str(),__FILE__, __LINE__));
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
std::shared_ptr<std::string> slim::module::import_specifier::get_source_code() const {
	return specifier_source_code_pointer;
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

void slim::module::import_specifier::compile_module() {
	log::trace(log::Message("slim::module::import_specifier::compile_module()",std::string("begins => " + specifier_string_url).c_str(),__FILE__, __LINE__));
	v8::TryCatch try_catch(isolate);
	v8::ScriptOrigin origin(utilities::StringToV8Value(isolate, specifier_path.string()), 0, 0, false, -1, utilities::StringToV8Value(isolate, ""), false, false, true);
	v8::ScriptCompiler::Source v8_module_source(utilities::StringToV8String(isolate, *specifier_source_code_pointer), origin);
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
	auto source_string_stream_pointer = slim::macros::apply(slim::common::fetch::stream(specifier_string), specifier_string);
	specifier_source_code_pointer = std::make_shared<std::string>(source_string_stream_pointer->str());
	specifier_original_source_code_pointer = std::make_shared<std::string>(source_string_stream_pointer->str());
/* 	std::regex from_pattern("[[:space:]\n]+from[[:space:]\n]+[\\\"\\'][.](\\/.+[^\"])[\\\"\\']");
	std::string expanded_path_statement =  " from '" + specifier_path_string + "$1'";
	specifier_source_code = std::regex_replace(specifier_source_code, from_pattern, expanded_path_statement); */
	log::debug(log::Message("slim::module::import_specifier::fetch_source()",std::string("specifier_string => " + specifier_string_url).c_str(), __FILE__, __LINE__));
	//log::debug(log::Message("slim::module::import_specifier::fetch_source()",std::string("source_string => " + specifier_source_code).c_str(), __FILE__, __LINE__));
	log::trace(log::Message("slim::module::import_specifier::fetch_source()",std::string("ends => " + specifier_string_url).c_str(),__FILE__, __LINE__));
}
void slim::module::import_specifier::instantiate_module() {
	log::trace(log::Message("slim::module::import_specifier::instantiate_module()",std::string("begins => " + specifier_string_url).c_str(),__FILE__, __LINE__));
	v8::TryCatch try_catch(isolate);
	auto result = v8_module->InstantiateModule(context, slim::module::resolver::module_call_back_resolver);
	if(result.IsNothing()) {
		log::error(log::Message("slim::module::import_specifier::instantiate_module()",std::string("v8_module->InstantiateModule() produced nothing => " + specifier_string_url).c_str(), __FILE__, __LINE__));
	}
	if(try_catch.HasCaught()) {
		log::error(log::Message("slim::module::import_specifier::instantiate_module()", "try_catch.HasCaught()",__FILE__, __LINE__));
		slim::exception_handler::v8_try_catch_handler(&try_catch);
	}
	log::debug(log::Message("slim::module::import_specifier::instantiate_module()",
		std::string("v8_module->InstantiateModule() status => " + get_module_status_string()).c_str(),__FILE__, __LINE__));
	log::trace(log::Message("slim::module::import_specifier::instantiate_module()",std::string("ends => " + specifier_string_url).c_str(),__FILE__, __LINE__));
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
					log::debug(log::Message("slim::module::import_specifier::import_specifier()", std::string("current_working_search_path exists() => " 
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