#include <filesystem>
#include <memory>
#include <regex>
#include <array>
#include <set>
#include <vector>
#include <v8.h>
#include <slim/common/exception.h>
#include <slim/common/log.h>
#include <slim/common/fetch_and_apply_macros.h>
#include <slim/gv8.h>
#include <slim/module_resolver.h>
#include <slim/plugin/loader.h>
#include <slim/utilities.h>


#include <iostream>

namespace slim::module::resolver {
	using namespace slim::utilities;
	using namespace slim::common::log;
	static std::set<std::string> plugins_set{"console", "fs", "kafka", "os", "path", "process", "node"};
	std::array<char*, 3> file_extensions = {".mjs", ".ts", ".js"};
	std::vector<std::string> search_paths = {
		"",
		"/home/greergan/product/slim/src/plugins/nodejs/lib/"
	};
	static specifier_cache_by_specifier by_specifier_cache;
	static specifier_cache_by_hash_id by_hash_id_cache;
}
slim::module::resolver::import_specifier::import_specifier() {}
slim::module::resolver::import_specifier::import_specifier(std::string specifier_string, v8::Local<v8::Context>& context, v8::Local<v8::Module> synthetic_module) 
																				: specifier_string(specifier_string), context(context), v8_module(synthetic_module) {
	trace(Message("slim::module::resolver::import_specifier::import_specifier()",std::string("begins => " + specifier_string).c_str(),__FILE__, __LINE__));
	isolate = context->GetIsolate();
	instantiate_module();
	v8_module->Evaluate(context).FromMaybe(v8::Local<v8::Value>());
	trace(Message("slim::module::resolver::import_specifier::import_specifier()",std::string("begins => " + specifier_string).c_str(),__FILE__, __LINE__));
}
slim::module::resolver::import_specifier::import_specifier(std::string specifier_string, v8::Local<v8::Context>& context, const bool is_entry_point_value, v8::Local<v8::Module> referrer)
																	: specifier_string(specifier_string), context(context), referrer(referrer), is_entry_point_value(is_entry_point_value) {
	trace(Message("slim::module::resolver::import_specifier::import_specifier()",std::string("begins => " + specifier_string).c_str(),__FILE__, __LINE__));
	isolate = context->GetIsolate();
	specifier_string_original = specifier_string;
	resolve_module_path();
	fetch_source();
	trace(Message("slim::module::resolver::import_specifier::import_specifier()",std::string("ends => " + this->get_specifier_url()).c_str(),__FILE__, __LINE__));
}
int slim::module::resolver::import_specifier::get_hash_id() {
	return v8_module->GetIdentityHash();
}
v8::Local<v8::Module>& slim::module::resolver::import_specifier::get_module() {
	return v8_module;
}
const std::string& slim::module::resolver::import_specifier::get_module_status_string() {
	switch(v8_module->GetStatus()) {
		case v8::Module::Status::kUninstantiated: v8_module_status = "v8::Module::Status::kUninstantiated"; break;
		case v8::Module::Status::kInstantiating: v8_module_status = "v8::Module::Status::kInstantiating"; break;
		case v8::Module::Status::kInstantiated: v8_module_status = "v8::Module::Status::kInstantiated"; break;
		case v8::Module::Status::kEvaluating: v8_module_status = "v8::Module::Status::kEvaluating"; break;
		case v8::Module::Status::kErrored: v8_module_status = "v8::Module::Status::kErrored"; break;
	}
	return v8_module_status;
}
const std::string& slim::module::resolver::import_specifier::get_specifier() const {
	return specifier_string;
}
const std::filesystem::path& slim::module::resolver::import_specifier::get_specifier_path() const {
	return specifier_path;
}
const std::string& slim::module::resolver::import_specifier::get_specifier_url() const {
	return specifier_string_url;
}
const bool slim::module::resolver::import_specifier::has_module() const {
	return has_module_value;
}
const bool slim::module::resolver::import_specifier::is_entry_point() const {
	return is_entry_point_value;
}
void slim::module::resolver::import_specifier::compile_module() {
	trace(Message("slim::module::resolver::import_specifier::compile_module()",std::string("begins => " + specifier_string_url).c_str(),__FILE__, __LINE__));
	v8::TryCatch try_catch(isolate);
	v8::ScriptOrigin origin(StringToValue(isolate, specifier_path.string()), 0, 0, false, -1, StringToValue(isolate, ""), false, false, true);
	v8::ScriptCompiler::Source v8_module_source(StringToString(isolate, specifier_source_code), origin);
	v8::ScriptCompiler::CompileOptions module_compile_options(v8::ScriptCompiler::kNoCompileOptions);
	v8::ScriptCompiler::NoCacheReason module_no_cache_reason(v8::ScriptCompiler::kNoCacheNoReason);
	v8::MaybeLocal<v8::Module> temporary_module = v8::ScriptCompiler::CompileModule(isolate, &v8_module_source, module_compile_options, module_no_cache_reason);
	if(!temporary_module.IsEmpty()) {
		v8_module = temporary_module.ToLocalChecked();
		has_module_value = true;
		debug(Message("slim::module::resolver::import_specifier::compile_module()", std::string("v8::ScriptCompiler::CompileModule() hash id => " + std::to_string(v8_module->GetIdentityHash())).c_str() , __FILE__, __LINE__));
	}
	if(try_catch.HasCaught()) {
		error(Message("slim::module::resolver::import_specifier::compile_module()", "try_catch.HasCaught()",__FILE__, __LINE__));
		slim::gv8::ReportException(&try_catch);
	}
	trace(Message("slim::module::resolver::import_specifier::compile_module()",std::string("ends => " + specifier_string_url).c_str(),__FILE__, __LINE__));
}
void slim::module::resolver::import_specifier::fetch_source() {
	trace(Message("slim::module::resolver::import_specifier::fetch_source()",std::string("begins file => " + specifier_string_url).c_str(),__FILE__, __LINE__));
	specifier_source_code = slim::common::fetch_and_apply_macros(specifier_string);
/* 	std::regex from_pattern("[[:space:]\n]+from[[:space:]\n]+[\\\"\\'][.](\\/.+[^\"])[\\\"\\']");
	std::string expanded_path_statement =  " from '" + specifier_path_string + "$1'";
	specifier_source_code = std::regex_replace(specifier_source_code, from_pattern, expanded_path_statement); */
	debug(Message("slim::module::resolver::import_specifier::fetch_source()",std::string("specifier_string => " + specifier_string_url).c_str(), __FILE__, __LINE__));
	//debug(Message("slim::module::resolver::import_specifier::fetch_source()",std::string("source_string => " + specifier_source_code).c_str(), __FILE__, __LINE__));
	trace(Message("slim::module::resolver::import_specifier::fetch_source()",std::string("ends file => " + specifier_string_url).c_str(),__FILE__, __LINE__));
}
void slim::module::resolver::import_specifier::instantiate_module() {
	trace(Message("slim::module::resolver::import_specifier::instantiate_module()",std::string("begins => " + specifier_string_url).c_str(),__FILE__, __LINE__));
	v8::TryCatch try_catch(isolate);
	info(Message("slim::module::resolver::import_specifier::instantiate_module()",
														std::string("v8_module->InstantiateModule() status => " + get_module_status_string()).c_str(),__FILE__, __LINE__));
	auto result = v8_module->InstantiateModule(context, module_call_back_resolver);
	if(result.IsNothing()) {
		error(Message("slim::module::resolver::import_specifier::instantiate_module()",std::string("v8_module->InstantiateModule() produced nothing => " + specifier_string_url).c_str(), __FILE__, __LINE__));
	}
	debug(Message("slim::module::resolver::import_specifier::instantiate_module()",
											std::string("v8_module->InstantiateModule() status => " + get_module_status_string()).c_str(),__FILE__, __LINE__));
	if(try_catch.HasCaught()) {
		error(Message("slim::module::resolver::import_specifier::instantiate_module()", "try_catch.HasCaught()",__FILE__, __LINE__));
		slim::gv8::ReportException(&try_catch);
	}
	trace(Message("slim::module::resolver::import_specifier::instantiate_module()",std::string("ends => " + specifier_string_url).c_str(),__FILE__, __LINE__));
}
void IterateFixedArray(v8::Isolate* isolate, v8::Local<v8::FixedArray> fixed_array) {
  	for(int i = 0; i < fixed_array->Length(); ++i) {
		v8::Local<v8::ModuleRequest> module_request = fixed_array->Get(isolate->GetCurrentContext(), i).As<v8::ModuleRequest>();
		std::cout << slim::utilities::v8StringToString(isolate, module_request->GetSpecifier()) << "\n";
		//module_request->GetImportAttributes()->Length();

/* 		v8::Local<v8::Data> element = fixed_array->Get(isolate->GetCurrentContext(), i);
		if(element->IsValue()) {
			std::cout << "element->IsValue()" << "\n";
		}
		else if(element->IsModule()) {
			std::cout << "element->IsModule()" << "\n";
		}
		else if(element->IsContext()) {
			std::cout << "element->IsContext()" << "\n";
		}
		else if(element->IsObjectTemplate()) {
			std::cout << "element->IsObjectTemplate()" << "\n";
		}
		else if(element->IsFunctionTemplate()) {
			std::cout << "element->IsFunctionTemplate()" << "\n";
		}
		else if(element->IsFixedArray()) {
			std::cout << "element->IsFixedArray()" << "\n";
		}
		else if(element->IsPrivate()) {
			std::cout << "element->IsPrivate()" << "\n";
		}
		else {
			std::cout << "unknown" << "\n";
		} */
		//std::cout << slim::utilities::v8StringToString(isolate, element.As<v8::Local<v8::String>>()) << "\n";
	}
}
 /* std::map<std::string, std::string> assertions_map;
  for (int i = 0; i < import_assertions->Length(); i += 2) {
    v8::Local<v8::String> key = import_assertions->Get(context, i).As<v8::String>();
    v8::Local<v8::String> value = import_assertions->Get(context, i + 1).As<v8::String>();
    v8::String::Utf8Value utf8_key(isolate, key);
    v8::String::Utf8Value utf8_value(isolate, value);
    assertions_map[*utf8_key] = *utf8_value;
  } */
v8::MaybeLocal<v8::Module> slim::module::resolver::module_call_back_resolver(
		v8::Local<v8::Context> context, v8::Local<v8::String> v8_specifier_name, v8::Local<v8::FixedArray> import_assertions, v8::Local<v8::Module> referrer) {
	trace(Message("slim::module::resolver::module_call_back_resolver()","begins", __FILE__, __LINE__));
	debug(Message("slim::module::resolver::module_call_back_resolver()",std::string("import_assertions->Length() => " + std::to_string(import_assertions->Length())).c_str(), __FILE__, __LINE__));
	auto isolate = context->GetIsolate();
	v8::TryCatch try_catch(isolate);
	std::string specifier_name_string = v8StringToString(isolate, v8_specifier_name);
	IterateFixedArray(isolate, referrer->GetModuleRequests());
	//debug(Message("slim::module::resolver::module_call_back_resolver()",std::string(specifier_name_string + " => referrer->GetModuleRequests()->Length() => " + std::to_string(referrer->GetModuleRequests()->Length())).c_str(), __FILE__, __LINE__));
	//debug(Message("slim::module::resolver::module_call_back_resolver()",std::string(specifier_name_string + " => referrer->GetIdentityHash() => " + std::to_string(referrer->GetIdentityHash())).c_str(), __FILE__, __LINE__));
/* 	if(module_specifier_cache[specifier_name_string].has_module()) {
		debug(Message("slim::module::resolver::module_call_back_resolver()",std::string("returning cached plugin => " + specifier_name_string).c_str(),__FILE__, __LINE__));
		auto t = module_specifier_cache[specifier_name_string].get_module();
		debug(Message("slim::module::resolver::module_call_back_resolver()",std::string(" plugin module status => " 
			+  std::to_string(t->GetStatus())).c_str(),__FILE__, __LINE__));
		
		
		return module_specifier_cache[specifier_name_string].get_module();
	} */
	int current_module_hash_id = -1;
	try {
		if(plugins_set.contains(specifier_name_string)) {
			debug(Message("slim::module::resolver::module_call_back_resolver()",std::string("loading => " + specifier_name_string).c_str(),__FILE__, __LINE__));
			if(!by_specifier_cache.contains(specifier_name_string)) {
				auto create_SyntheticModuleEvaluationSteps = [](v8::Local<v8::Context> context, v8::Local<v8::Module> module) -> v8::MaybeLocal<v8::Value> {
					auto isolate = context->GetIsolate();
					v8::TryCatch try_catch(isolate);
					auto plugin_name_string = v8ValueToString(isolate, context->GetEmbedderData(0));
					debug(Message("slim::module::resolver::module_call_back_resolver()",std::string("create_SyntheticModuleEvaluationSteps => " + plugin_name_string).c_str(),__FILE__, __LINE__));
					auto plugin_v8_object = GetObject(isolate, plugin_name_string, context->Global());
					module->SetSyntheticModuleExport(isolate, StringToV8String(isolate, "default"), plugin_v8_object);
					auto property_names_array = plugin_v8_object->GetOwnPropertyNames(context);
					if(!property_names_array.IsEmpty()) {
						auto property_names_array_local = property_names_array.ToLocalChecked();
						for(int array_index = 0; array_index < property_names_array_local->Length(); array_index++) {
							auto v8_property_name_string = property_names_array_local->Get(context, array_index).ToLocalChecked()->ToString(context);
							if(!v8_property_name_string.IsEmpty()) {
								auto v8_property_value = plugin_v8_object->Get(context, v8_property_name_string.ToLocalChecked());
								if(!v8_property_value.IsEmpty()) {
									module->SetSyntheticModuleExport(isolate, v8_property_name_string.ToLocalChecked(), v8_property_value.ToLocalChecked());
								}
							}
						}
					}
					if(try_catch.HasCaught()) {
						error(Message("slim::module::resolver::module_call_back_resolver()", "try_catch.HasCaught()",__FILE__, __LINE__));
						slim::gv8::ReportException(&try_catch);
					}
					return v8::MaybeLocal<v8::Value>(True(isolate));
				};
				debug(Message("slim::module::resolver::module_call_back_resolver()",std::string("loading plugin => " + specifier_name_string).c_str(),__FILE__, __LINE__));
				slim::plugin::loader::load_plugin(isolate, specifier_name_string, true);
				debug(Message("slim::module::resolver::module_call_back_resolver()",std::string("loaded plugin => " + specifier_name_string).c_str(),__FILE__, __LINE__));
				if(try_catch.HasCaught()) {
					auto message = slim::utilities::v8StringToString(isolate, try_catch.Message()->Get());
					error(Message("slim::module::resolver::module_call_back_resolver()", std::string("try_catch.HasCaught() => " + message).c_str(),__FILE__, __LINE__));
					isolate->ThrowError(try_catch.Message()->Get());
					//slim::gv8::ReportException(&try_catch);
				}
				const v8::Local<v8::String> v8_default_string = StringToV8String(isolate, "default");
				std::vector<v8::Local<v8::String>> v8_string_exports_vector;
				v8_string_exports_vector.push_back(v8_default_string);
				V8KeysToVector(isolate, v8_string_exports_vector, GetObject(isolate, specifier_name_string, context->Global()));
				context->SetEmbedderData(0, v8_specifier_name); //needed in create_SyntheticModuleEvaluationSteps
				const v8::MemorySpan<const v8::Local<v8::String>> memory_span(v8_string_exports_vector.data(), v8_string_exports_vector.size());
				debug(Message("slim::module::resolver::module_call_back_resolver()", "setting synthetic plugin", __FILE__, __LINE__));
				import_specifier module_specifier(specifier_name_string, context, v8::Module::CreateSyntheticModule(isolate,
									StringToV8String(isolate, specifier_name_string), memory_span, create_SyntheticModuleEvaluationSteps));
				cache_import_specifier(std::make_shared<import_specifier>(module_specifier));
				current_module_hash_id = module_specifier.get_hash_id();
				debug(Message("slim::module::resolver::module_call_back_resolver()", "done setting synthetic plugin", __FILE__, __LINE__));
				if(try_catch.HasCaught()) {
					error(Message("slim::module::resolver::module_call_back_resolver()", "try_catch.HasCaught()",__FILE__, __LINE__));
					slim::gv8::ReportException(&try_catch);
				}
			}
		}
		else {
			debug(Message("slim::module::resolver::module_call_back_resolver()", std::string("loading file => " + specifier_name_string).c_str(),__FILE__, __LINE__));
			debug(Message("slim::module::resolver::module_call_back_resolver()", std::string("referring module id => " + std::to_string(referrer->GetIdentityHash())).c_str(),__FILE__, __LINE__));
			import_specifier module_specifier(specifier_name_string, context, false, referrer);
			debug(Message("slim::module::resolver::module_call_back_resolver()", std::string("specifier_name_string => " + module_specifier.get_specifier_url()).c_str(),__FILE__, __LINE__));
			module_specifier.compile_module(); // 1st compile
			cache_import_specifier(std::make_shared<import_specifier>(module_specifier)); // 2nd store it
			module_specifier.instantiate_module(); // 3rd instantiate it which causes a recursive module import chain of events
			current_module_hash_id = module_specifier.get_hash_id();
		}
	}
    catch(const slim::common::SlimFileException& _error) {
        std::string error_message = _error.message + ", path => " + _error.path;
        error(Message(_error.call.c_str(), error_message.c_str(),__FILE__, __LINE__));
		isolate->ThrowException(StringToV8String(isolate, "Module not found: " + specifier_name_string));
    }
/* 	if(try_catch.HasCaught()) {
		error(Message("slim::module::resolver::module_call_back_resolver()", "try_catch.HasCaught()",__FILE__, __LINE__));
		slim::gv8::ReportException(&try_catch);
	} */
	for(auto& [id, specifier] : by_hash_id_cache) {
		std::cout << "hash id => " << id << ", specifier => " << specifier->get_specifier() <<"\n";
	}
	if(by_hash_id_cache.contains(current_module_hash_id)) {
		trace(Message("slim::module::resolver::module_call_back_resolver()",
			std::string("ends => " + by_hash_id_cache[current_module_hash_id]->get_specifier_url() + " => " + std::to_string(current_module_hash_id)).c_str(), __FILE__, __LINE__));
		return by_hash_id_cache[current_module_hash_id]->get_module();
	}
	trace(Message("slim::module::resolver::module_call_back_resolver()",std::string("ends empty module => " + specifier_name_string).c_str(), __FILE__, __LINE__));
	return v8::MaybeLocal<v8::Module>();
}
std::shared_ptr<slim::module::resolver::import_specifier> slim::module::resolver::resolve_imports(
											std::string entry_script_file_name_string_in, v8::Local<v8::Context> context, const bool is_entry_point_value = false) {
	trace(Message("slim::module::resolver::resolve_imports()",std::string("begins => " + entry_script_file_name_string_in).c_str(), __FILE__, __LINE__));
	import_specifier entry_script_specifier(entry_script_file_name_string_in, context, is_entry_point_value, v8::Local<v8::Module>());
	entry_script_specifier.compile_module(); // compile module so we can get at the hash id during module imports
	cache_import_specifier(std::make_shared<import_specifier>(entry_script_specifier)); // now cache it before instantiate_module
	entry_script_specifier.instantiate_module(); // instantiate_module causes import recursion where we need hash id to get at the parent path of current import
	trace(Message("slim::module::resolver::resolve_imports()",std::string("ends => " + entry_script_specifier.get_specifier_url()).c_str(), __FILE__, __LINE__));
	return by_specifier_cache[entry_script_specifier.get_specifier()];
}
void slim::module::resolver::import_specifier::resolve_module_path() {
	trace(Message("slim::module::resolver::resolve_module_path()",std::string("begins => " + specifier_string).c_str(), __FILE__, __LINE__));
	v8::TryCatch try_catch(isolate);
	bool module_file_found = false;
	if(!specifier_string.starts_with("./") && !specifier_string.starts_with("/")) {
		debug(Message("slim::module::resolver::resolve_module_path()",std::string("specifier_string !starts with ./ or / => " + specifier_string).c_str(), __FILE__, __LINE__));
		for(auto& current_search_path : search_paths) {
			auto current_working_search_path = std::filesystem::absolute(current_search_path + specifier_string);
			debug(Message("slim::module::resolver::import_specifier::import_specifier()",current_working_search_path.string().c_str(),__FILE__, __LINE__));
			debug(Message("slim::module::resolver::import_specifier::import_specifier()",std::string("current_working_search_path.has_extension() => " 
																+ std::to_string(current_working_search_path.has_extension())).c_str(),__FILE__, __LINE__));
			if(current_working_search_path.has_extension()) {
				if(std::filesystem::exists(current_working_search_path)) {
					debug(Message("slim::module::resolver::import_specifier::import_specifier()",std::string("current_working_search_path exists() => " 
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
							debug(Message("slim::module::resolver::import_specifier::import_specifier()",
								std::string("did not find => " + possible_module_file_path.string()).c_str(),__FILE__, __LINE__));
						}
					}
				}
			}
		}
	}
	else {
		if(by_hash_id_cache.contains(referrer->GetIdentityHash())) {
			auto parent_path = by_hash_id_cache[referrer->GetIdentityHash()]->get_specifier_path().parent_path().string();
			if(specifier_string.starts_with("./")) {
				specifier_path = std::filesystem::absolute(parent_path + "/" + specifier_string.substr(2));
			}
		}
		else {
			specifier_path = specifier_string.starts_with(".") ? std::filesystem::absolute(specifier_string.substr(2)) : std::filesystem::absolute(specifier_string); 
		}
		debug(Message("slim::module::resolver::import_specifier::import_specifier()",std::string("specifier_path.has_extension() => " +  std::string(specifier_path.has_extension() ? "true" : "false")).c_str(),__FILE__, __LINE__));
		debug(Message("slim::module::resolver::import_specifier::import_specifier()",std::string("specifier_path.string() => " +  specifier_path.string()).c_str(),__FILE__, __LINE__));
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
		error(Message("slim::module::resolver::import_specifier::import_specifier()",std::string("module not found => " + specifier_string).c_str(),__FILE__, __LINE__));
		isolate->ThrowException(StringToV8String(isolate, "module not found => " + specifier_string));
	}
	else {
		specifier_string = specifier_path.string();
		specifier_string_url = "file://" + specifier_string;
	}
/* 	if(try_catch.HasCaught()) {
		error(Message("slim::module::resolver::import_specifier::import_specifier()",std::string("module not found => " + 
			v8ValueToString(isolate, try_catch.Exception())).c_str(),__FILE__, __LINE__));
		slim::gv8::ReportException(&try_catch);
	} */
	trace(Message("slim::module::resolver::resolve_module_path()",std::string("ends => " + specifier_string_url).c_str(), __FILE__, __LINE__));
}
static void slim::module::resolver::cache_import_specifier(std::shared_ptr<import_specifier> module_import_specifier) {
	by_specifier_cache[module_import_specifier->get_specifier()] = module_import_specifier;
	by_hash_id_cache[module_import_specifier->get_module()->GetIdentityHash()] = module_import_specifier;
}