#include <filesystem>
#include <memory>
#include <regex>
#include <array>
#include <set>
#include <unordered_map>
#include <vector>
#include <v8.h>
#include <slim/builtins/typescript.h>
#include <slim/common/exception.h>
#include <slim/common/log.h>
#include <slim/common/fetch_and_apply_macros.h>
#include <slim/macros.h>
#include <slim/gv8.h>
#include <slim/module_resolver.h>
#include <slim/plugin/loader.h>
#include <slim/utilities.h>


#include <iostream>

namespace slim::module::resolver {
	using namespace slim;
	using namespace slim::common;
	static std::set<std::string> plugins_set{"console", "fs", "kafka", "os", "path", "process", "memoryAdaptor"};
	std::array<char*, 3> file_extensions = {".mjs", ".ts", ".js"};
	std::vector<std::string> search_paths = {
		""
	};
	static specifier_cache_by_specifier by_specifier_cache;
	static specifier_cache_by_hash_id by_hash_id_cache;
	std::string typescript_compiled_specifier_string_url;
	void receive_compiled_file_name(const v8::FunctionCallbackInfo<v8::Value>& args);
}
slim::module::resolver::import_specifier::import_specifier() {}
slim::module::resolver::import_specifier::import_specifier(std::string specifier_string, v8::Local<v8::Context>& context, v8::Local<v8::Module> synthetic_module) 
																				: specifier_string(specifier_string), context(context), v8_module(synthetic_module) {
	log::trace(log::Message("slim::module::resolver::import_specifier::import_specifier()",std::string("begins => " + specifier_string).c_str(),__FILE__, __LINE__));
	isolate = context->GetIsolate();
	instantiate_module();
	v8_module->Evaluate(context).FromMaybe(v8::Local<v8::Value>());
	log::trace(log::Message("slim::module::resolver::import_specifier::import_specifier()",std::string("ends => " + specifier_string).c_str(),__FILE__, __LINE__));
}
slim::module::resolver::import_specifier::import_specifier(std::string specifier_string, v8::Local<v8::Context>& context, const bool is_entry_point_value, v8::Local<v8::Module> referrer)
																	: specifier_string(specifier_string), context(context), referrer(referrer), is_entry_point_value(is_entry_point_value) {
	log::trace(log::Message("slim::module::resolver::import_specifier::import_specifier()",std::string("begins => " + specifier_string).c_str(),__FILE__, __LINE__));
	isolate = context->GetIsolate();
	specifier_string_original = specifier_string;
	auto source_file_content_string_pointer = builtins::typescript::get_file_content_pointer(specifier_string);
	if(!source_file_content_string_pointer->empty()) {
		specifier_source_code = macros::apply(source_file_content_string_pointer, specifier_string);
		log::debug(log::Message("slim::module::resolver::import_specifier::import_specifier()",std::string("received source for => " + specifier_string).c_str(),__FILE__, __LINE__));
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
	log::trace(log::Message("slim::module::resolver::import_specifier::import_specifier()",std::string("ends => " + this->get_specifier_url()).c_str(),__FILE__, __LINE__));
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
const std::string& slim::module::resolver::import_specifier::get_source_code() const {
	return specifier_source_code;
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
void slim::module::resolver::import_specifier::set_source_code(std::string source_code) {
	specifier_original_source_code = specifier_source_code;
	specifier_source_code = source_code;
}
void slim::module::resolver::import_specifier::compile_module() {
	log::trace(log::Message("slim::module::resolver::import_specifier::compile_module()",std::string("begins => " + specifier_string_url).c_str(),__FILE__, __LINE__));
	v8::TryCatch try_catch(isolate);
	v8::ScriptOrigin origin(utilities::StringToV8Value(isolate, specifier_path.string()), 0, 0, false, -1, utilities::StringToV8Value(isolate, ""), false, false, true);
	v8::ScriptCompiler::Source v8_module_source(utilities::StringToV8String(isolate, specifier_source_code), origin);
	v8::ScriptCompiler::CompileOptions module_compile_options(v8::ScriptCompiler::kProduceCompileHints);
	v8::ScriptCompiler::NoCacheReason module_no_cache_reason(v8::ScriptCompiler::kNoCacheNoReason);
	v8::MaybeLocal<v8::Module> temporary_module = v8::ScriptCompiler::CompileModule(isolate, &v8_module_source, module_compile_options, module_no_cache_reason);
	if(!temporary_module.IsEmpty()) {
		v8_module = temporary_module.ToLocalChecked();
		has_module_value = true;
		log::debug(log::Message("slim::module::resolver::import_specifier::compile_module()", std::string("v8::ScriptCompiler::CompileModule() hash id => " + std::to_string(v8_module->GetIdentityHash())).c_str() , __FILE__, __LINE__));
	}
	if(try_catch.HasCaught()) {
		log::error(log::Message("slim::module::resolver::import_specifier::compile_module()", "try_catch.HasCaught()",__FILE__, __LINE__));
		slim::gv8::ReportException(&try_catch);
	}
	log::trace(log::Message("slim::module::resolver::import_specifier::compile_module()",std::string("ends => " + specifier_string_url).c_str(),__FILE__, __LINE__));
}
void slim::module::resolver::import_specifier::fetch_source() {
	log::trace(log::Message("slim::module::resolver::import_specifier::fetch_source()",std::string("begins => " + specifier_string_url).c_str(),__FILE__, __LINE__));
	specifier_source_code = slim::common::fetch_and_apply_macros(specifier_string);
/* 	std::regex from_pattern("[[:space:]\n]+from[[:space:]\n]+[\\\"\\'][.](\\/.+[^\"])[\\\"\\']");
	std::string expanded_path_statement =  " from '" + specifier_path_string + "$1'";
	specifier_source_code = std::regex_replace(specifier_source_code, from_pattern, expanded_path_statement); */
	log::debug(log::Message("slim::module::resolver::import_specifier::fetch_source()",std::string("specifier_string => " + specifier_string_url).c_str(), __FILE__, __LINE__));
	//log::debug(log::Message("slim::module::resolver::import_specifier::fetch_source()",std::string("source_string => " + specifier_source_code).c_str(), __FILE__, __LINE__));
	log::trace(log::Message("slim::module::resolver::import_specifier::fetch_source()",std::string("ends => " + specifier_string_url).c_str(),__FILE__, __LINE__));
}
void slim::module::resolver::import_specifier::instantiate_module() {
	log::trace(log::Message("slim::module::resolver::import_specifier::instantiate_module()",std::string("begins => " + specifier_string).c_str(),__FILE__, __LINE__));
	v8::TryCatch try_catch(isolate);
	auto result = v8_module->InstantiateModule(context, module_call_back_resolver);
	if(result.IsNothing()) {
		log::error(log::Message("slim::module::resolver::import_specifier::instantiate_module()",std::string("v8_module->InstantiateModule() produced nothing => " + specifier_string).c_str(), __FILE__, __LINE__));
	}
	if(try_catch.HasCaught()) {
		log::error(log::Message("slim::module::resolver::import_specifier::instantiate_module()", "try_catch.HasCaught()",__FILE__, __LINE__));
		slim::gv8::ReportException(&try_catch);
	}
	log::debug(log::Message("slim::module::resolver::import_specifier::instantiate_module()",
											std::string("v8_module->InstantiateModule() status => " + get_module_status_string()).c_str(),__FILE__, __LINE__));
	log::trace(log::Message("slim::module::resolver::import_specifier::instantiate_module()",std::string("ends => " + specifier_string).c_str(),__FILE__, __LINE__));
}
void slim::module::resolver::import_specifier::initialize_typescript_compiler() {
	log::trace(log::Message("slim::module::resolver::initialize_typescript_compiler()", std::string("begins => " + specifier_string_url).c_str(),__FILE__, __LINE__));
	int typescript_specifier_hash_id = builtins::typescript::get_specifier_module_hash_id();
	log::debug(log::Message("slim::module::resolver::initialize_typescript_compiler()", std::to_string(typescript_specifier_hash_id).c_str(),__FILE__, __LINE__));
	auto typescript_import_specifier = by_hash_id_cache[typescript_specifier_hash_id];
	log::debug(log::Message("slim::module::resolver::initialize_typescript_compiler()", "got import_specifier",__FILE__, __LINE__));
	auto typescript_module_namespace_object = typescript_import_specifier->get_module()->GetModuleNamespace()->ToObject(context).ToLocalChecked();
	log::debug(log::Message("slim::module::resolver::initialize_typescript_compiler()", "got namespace object",__FILE__, __LINE__));
	auto v8_function_name_string = utilities::StringToV8String(isolate, "compile");
	v8::Local<v8::Value> v8_function_value;
	if(!typescript_module_namespace_object->Get(context, v8_function_name_string).ToLocal(&v8_function_value)) {
		std::cerr << "Error getting function." << std::endl;
	}
	log::debug(log::Message("slim::module::resolver::initialize_typescript_compiler()", "got v8_function_value object",__FILE__, __LINE__));
	typescript_compile_function = v8::Local<v8::Function>::Cast(v8_function_value);
	log::debug(log::Message("slim::module::resolver::initialize_typescript_compiler()", "got typescript_compile_function",__FILE__, __LINE__));
	if(typescript_compile_function.IsEmpty()) {
		std::cerr << "typescript_compile_function.IsEmpty()" << std::endl;
	}
	is_typescript_compiler_initialized = true;
	log::trace(log::Message("slim::module::resolver::initialize_typescript_compiler()", std::string("ends => " + specifier_string_url).c_str(),__FILE__, __LINE__));
}
void slim::module::resolver::receive_compiled_file_name(const v8::FunctionCallbackInfo<v8::Value>& args) {
	typescript_compiled_specifier_string_url = utilities::v8ValueToString(args.GetIsolate(), args[0]);
}
void slim::module::resolver::import_specifier::call_typescript_compiler() {
	log::trace(log::Message("slim::module::resolver::call_typescript_compiler()", std::string("begins => " + specifier_string_url).c_str(),__FILE__, __LINE__));
	auto receive_compiled_file_name_function_template = v8::FunctionTemplate::New(isolate, slim::module::resolver::receive_compiled_file_name);
	auto receive_compiled_file_name_function = receive_compiled_file_name_function_template->GetFunction(context).ToLocalChecked();
    const int argc = 2;
    v8::Local<v8::Value> args[argc] = {utilities::StringToV8String(isolate, specifier_string_url),receive_compiled_file_name_function};
	auto call_result = typescript_compile_function->Call(context, context->Global(), argc, args).ToLocalChecked();
	log::debug(log::Message("slim::module::resolver::run_typescript()", std::string("have called typescript_compile_function => " + specifier_string_url).c_str(),__FILE__, __LINE__));
	if(call_result.IsEmpty()) {
		std::cerr << "Error calling function." << std::endl;
	}
	log::trace(log::Message("slim::module::resolver::call_typescript_compiler()", std::string("ends => " + specifier_string_url).c_str(),__FILE__, __LINE__));
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
	auto isolate = context->GetIsolate();
	log::trace(log::Message("slim::module::resolver::module_call_back_resolver()",std::string("begins => " + utilities::v8StringToString(isolate, v8_specifier_name)).c_str(), __FILE__, __LINE__));
	log::debug(log::Message("slim::module::resolver::module_call_back_resolver()",std::string("import_assertions->Length() => " + std::to_string(import_assertions->Length())).c_str(), __FILE__, __LINE__));
	v8::TryCatch try_catch(isolate);
	std::string specifier_name_string = utilities::v8StringToString(isolate, v8_specifier_name);
	//IterateFixedArray(isolate, referrer->GetModuleRequests());
	//log::debug(log::Message("slim::module::resolver::module_call_back_resolver()",std::string(specifier_name_string + " => referrer->GetModuleRequests()->Length() => " + std::to_string(referrer->GetModuleRequests()->Length())).c_str(), __FILE__, __LINE__));
	//log::debug(log::Message("slim::module::resolver::module_call_back_resolver()",std::string(specifier_name_string + " => referrer->GetIdentityHash() => " + std::to_string(referrer->GetIdentityHash())).c_str(), __FILE__, __LINE__));
/* 	if(module_specifier_cache[specifier_name_string].has_module()) {
		log::debug(log::Message("slim::module::resolver::module_call_back_resolver()",std::string("returning cached plugin => " + specifier_name_string).c_str(),__FILE__, __LINE__));
		auto t = module_specifier_cache[specifier_name_string].get_module();
		log::debug(log::Message("slim::module::resolver::module_call_back_resolver()",std::string(" plugin module status => " 
			+  std::to_string(t->GetStatus())).c_str(),__FILE__, __LINE__));
		
		
		return module_specifier_cache[specifier_name_string].get_module();
	} */
	int current_module_hash_id = -1;
	try {
		if(plugins_set.contains(specifier_name_string)) {
			log::debug(log::Message("slim::module::resolver::module_call_back_resolver()",std::string("loading => " + specifier_name_string).c_str(),__FILE__, __LINE__));
			if(by_specifier_cache.contains(specifier_name_string)) {
				for(auto& [id, specifier] : by_hash_id_cache) {
					if(specifier->get_specifier() == specifier_name_string) {
						by_hash_id_cache.erase(id);
						by_specifier_cache.erase(specifier_name_string);
						break;
					}
				}
			}
			{
				auto create_SyntheticModuleEvaluationSteps = [](v8::Local<v8::Context> context, v8::Local<v8::Module> module) -> v8::MaybeLocal<v8::Value> {
					auto isolate = context->GetIsolate();
					v8::TryCatch try_catch(isolate);
					auto plugin_name_string = utilities::v8ValueToString(isolate, context->GetEmbedderData(0));
					log::debug(log::Message("slim::module::resolver::module_call_back_resolver()",std::string("create_SyntheticModuleEvaluationSteps => " + plugin_name_string).c_str(),__FILE__, __LINE__));
					auto plugin_v8_object = utilities::GetObject(isolate, plugin_name_string, context->Global());
					module->SetSyntheticModuleExport(isolate, utilities::StringToV8String(isolate, "default"), plugin_v8_object);
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
						log::error(log::Message("slim::module::resolver::module_call_back_resolver()", "try_catch.HasCaught()",__FILE__, __LINE__));
						slim::gv8::ReportException(&try_catch);
					}
					return v8::MaybeLocal<v8::Value>(True(isolate));
				};
				log::debug(log::Message("slim::module::resolver::module_call_back_resolver()",std::string("loading plugin => " + specifier_name_string).c_str(),__FILE__, __LINE__));
				slim::plugin::loader::load_plugin(isolate, specifier_name_string, true);
				log::debug(log::Message("slim::module::resolver::module_call_back_resolver()",std::string("loaded plugin => " + specifier_name_string).c_str(),__FILE__, __LINE__));
				if(try_catch.HasCaught()) {
					auto message = slim::utilities::v8StringToString(isolate, try_catch.Message()->Get());
					log::error(log::Message("slim::module::resolver::module_call_back_resolver()", std::string("try_catch.HasCaught() => " + message).c_str(),__FILE__, __LINE__));
					isolate->ThrowError(try_catch.Message()->Get());
					//slim::gv8::ReportException(&try_catch);
				}
				const v8::Local<v8::String> v8_default_string = utilities::StringToV8String(isolate, "default");
				std::vector<v8::Local<v8::String>> v8_string_exports_vector;
				v8_string_exports_vector.push_back(v8_default_string);
				utilities::V8KeysToVector(isolate, v8_string_exports_vector, utilities::GetObject(isolate, specifier_name_string, context->Global()));
				context->SetEmbedderData(0, v8_specifier_name); //needed in create_SyntheticModuleEvaluationSteps
				const v8::MemorySpan<const v8::Local<v8::String>> memory_span(v8_string_exports_vector.data(), v8_string_exports_vector.size());
				log::debug(log::Message("slim::module::resolver::module_call_back_resolver()", "setting synthetic plugin", __FILE__, __LINE__));
				import_specifier module_specifier(specifier_name_string, context, v8::Module::CreateSyntheticModule(isolate,
									utilities::StringToV8String(isolate, specifier_name_string), memory_span, create_SyntheticModuleEvaluationSteps));
				cache_import_specifier(std::make_shared<import_specifier>(module_specifier));
				current_module_hash_id = module_specifier.get_hash_id();
				log::debug(log::Message("slim::module::resolver::module_call_back_resolver()", "done setting synthetic plugin", __FILE__, __LINE__));
				if(try_catch.HasCaught()) {
					log::error(log::Message("slim::module::resolver::module_call_back_resolver()", "try_catch.HasCaught()",__FILE__, __LINE__));
					slim::gv8::ReportException(&try_catch);
				}
			}
		}
		else {
			import_specifier module_specifier(specifier_name_string, context, false, referrer);
			module_specifier.compile_module(); // compile module so we can get at the hash id during module imports
			log::trace(log::Message("slim::module::resolver::module_call_back_resolver()", std::string("get_module_status_string() => " + module_specifier.get_module_status_string()).c_str(),__FILE__, __LINE__));
			if(module_specifier.get_module()->GetStatus() == v8::Module::Status::kErrored) {
				isolate->ThrowException(module_specifier.get_module()->GetException());
			}
			else {
				cache_import_specifier(std::make_shared<import_specifier>(module_specifier)); // now cache it before instantiate_module
				module_specifier.instantiate_module(); // instantiate_module causes import recursion where we need hash id to get at the parent path of current import
				if(module_specifier.get_module()->GetStatus() == v8::Module::Status::kErrored) {
					isolate->ThrowException(module_specifier.get_module()->GetException());
				}
				current_module_hash_id = module_specifier.get_hash_id();
				log::debug(log::Message("slim::module::resolver::module_call_back_resolver()", std::string("get_module_status_string() => " + module_specifier.get_module_status_string()).c_str(),__FILE__, __LINE__));
			}
		}
	}
    catch(const slim::common::SlimFileException& _error) {
        std::string error_message = _error.message + ", path => " + _error.path;
        log::error(log::Message(_error.call.c_str(), error_message.c_str(),__FILE__, __LINE__));
		isolate->ThrowException(utilities::StringToV8String(isolate, "Module not found: " + specifier_name_string));
    }
	catch (...) {
		log::error(log::Message("slim::module::resolver::module_call_back_resolver()",std::string("caught unknown error => " + specifier_name_string).c_str(), __FILE__, __LINE__));
	}
	if(by_hash_id_cache.contains(current_module_hash_id)) {
		log::trace(log::Message("slim::module::resolver::module_call_back_resolver()",
			std::string("ends => " + (by_hash_id_cache[current_module_hash_id]->get_specifier_url().length() > 0 
							? by_hash_id_cache[current_module_hash_id]->get_specifier_url() : by_hash_id_cache[current_module_hash_id]->get_specifier()) 
							+ " => " + std::to_string(current_module_hash_id)).c_str(), __FILE__, __LINE__));
		return by_hash_id_cache[current_module_hash_id]->get_module();
	}

		isolate->ThrowError(utilities::StringToV8String(isolate, "Module is empty: " + specifier_name_string));
		isolate->ThrowException(utilities::StringToV8String(isolate, "Module is empty: " + specifier_name_string));
	if(try_catch.HasCaught()) {
		log::error(log::Message("slim::module::resolver::module_call_back_resolver()", "try_catch.HasCaught()",__FILE__, __LINE__));
		log::error(log::Message("slim::module::resolver::module_call_back_resolver()", utilities::v8ValueToString(isolate, try_catch.Exception()).c_str(),__FILE__, __LINE__));
		slim::gv8::ReportException(&try_catch);
	}
	log::error(log::Message("slim::module::resolver::module_call_back_resolver()",std::string("ends empty module => " + specifier_name_string).c_str(), __FILE__, __LINE__));
/* 	const size_t frameLimit = 100; // Set a limit for the number of frames to capture
    v8::Local<v8::StackTrace> stackTrace = v8::StackTrace::CurrentStackTrace(isolate, frameLimit);
    int frameCount = stackTrace->GetFrameCount();
    for (int i = 0; i < frameCount; ++i) {
        v8::Local<v8::StackFrame> frame = stackTrace->GetFrame(isolate, i);
        v8::Local<v8::String> functionName = frame->GetFunctionName();
        v8::Local<v8::String> scriptName = frame->GetScriptName();
        int lineNumber = frame->GetLineNumber();
        int column = frame->GetColumn();
		log::error(log::Message(utilities::v8StringToString(isolate, scriptName).c_str(),utilities::v8StringToString(isolate, functionName).c_str(),__FILE__, __LINE__));
    } */

	//return v8::MaybeLocal<v8::Module>();
}
std::shared_ptr<slim::module::resolver::import_specifier> slim::module::resolver::resolve_imports(
											std::string entry_script_file_name_string_in, v8::Local<v8::Context> context, const bool is_entry_point_value = false) {
	log::trace(log::Message("slim::module::resolver::resolve_imports()",std::string("begins => " + entry_script_file_name_string_in).c_str(), __FILE__, __LINE__));
	import_specifier entry_script_specifier(entry_script_file_name_string_in, context, is_entry_point_value, v8::Local<v8::Module>());
	entry_script_specifier.compile_module(); // compile module so we can get at the hash id during module imports
	cache_import_specifier(std::make_shared<import_specifier>(entry_script_specifier)); // now cache it before instantiate_module
	entry_script_specifier.instantiate_module(); // instantiate_module causes import recursion where we need hash id to get at the parent path of current import
	log::trace(log::Message("slim::module::resolver::resolve_imports()",std::string("ends => " + entry_script_specifier.get_specifier_url()).c_str(), __FILE__, __LINE__));
	return by_specifier_cache[entry_script_specifier.get_specifier()];
}
void slim::module::resolver::import_specifier::resolve_module_path() {
	log::trace(log::Message("slim::module::resolver::resolve_module_path()",std::string("begins => " + specifier_string).c_str(), __FILE__, __LINE__));
	v8::TryCatch try_catch(isolate);
	bool module_file_found = false;
	if(!specifier_string.starts_with("./") && !specifier_string.starts_with("/")) {
		log::debug(log::Message("slim::module::resolver::resolve_module_path()",std::string("specifier_string !starts with ./ or / => " + specifier_string).c_str(), __FILE__, __LINE__));
		for(auto& current_search_path : search_paths) {
			auto current_working_search_path = std::filesystem::absolute(current_search_path + specifier_string);
			log::debug(log::Message("slim::module::resolver::import_specifier::import_specifier()",current_working_search_path.string().c_str(),__FILE__, __LINE__));
			log::debug(log::Message("slim::module::resolver::import_specifier::import_specifier()",std::string("current_working_search_path.has_extension() => " 
																+ std::to_string(current_working_search_path.has_extension())).c_str(),__FILE__, __LINE__));
			if(current_working_search_path.has_extension()) {
				if(std::filesystem::exists(current_working_search_path)) {
					log::debug(log::Message("slim::module::resolver::import_specifier::import_specifier()",std::string("current_working_search_path exists() => " 
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
							log::debug(log::Message("slim::module::resolver::import_specifier::import_specifier()",
								std::string("did not find => " + possible_module_file_path.string()).c_str(),__FILE__, __LINE__));
						}
					}
				}
			}
		}
	}
	else {
		log::debug(log::Message("slim::module::resolver::import_specifier::import_specifier()",std::string("specifier_string => " +  specifier_string).c_str(),__FILE__, __LINE__));
		if(!referrer.IsEmpty() > 0 && by_hash_id_cache.contains(referrer->GetIdentityHash())) {
			log::debug(log::Message("slim::module::resolver::import_specifier::import_specifier()",std::string("specifier_string => " +  specifier_string).c_str(),__FILE__, __LINE__));
			auto parent_path = by_hash_id_cache[referrer->GetIdentityHash()]->get_specifier_path().parent_path().string();
			log::debug(log::Message("slim::module::resolver::import_specifier::import_specifier()",std::string("specifier_string => " +  specifier_string).c_str(),__FILE__, __LINE__));
			if(specifier_string.starts_with("./")) {
				log::debug(log::Message("slim::module::resolver::import_specifier::import_specifier()",std::string("specifier_string => " +  specifier_string).c_str(),__FILE__, __LINE__));
				specifier_path = std::filesystem::absolute(parent_path + "/" + specifier_string.substr(2));
				log::debug(log::Message("slim::module::resolver::import_specifier::import_specifier()",std::string("specifier_path.string() => " +  specifier_path.string()).c_str(),__FILE__, __LINE__));
			}
			log::debug(log::Message("slim::module::resolver::import_specifier::import_specifier()",std::string("specifier_string => " +  specifier_string).c_str(),__FILE__, __LINE__));
		}
		else {
			log::debug(log::Message("slim::module::resolver::import_specifier::import_specifier()",std::string("specifier_string => " +  specifier_string).c_str(),__FILE__, __LINE__));
			specifier_path = specifier_string.starts_with(".") ? std::filesystem::absolute(specifier_string.substr(2)) : std::filesystem::absolute(specifier_string);
		}
		log::debug(log::Message("slim::module::resolver::import_specifier::import_specifier()",std::string("specifier_string => " +  specifier_string).c_str(),__FILE__, __LINE__));
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
		log::error(log::Message("slim::module::resolver::import_specifier::import_specifier()",std::string("module not found => " + specifier_string).c_str(),__FILE__, __LINE__));
		isolate->ThrowException(utilities::StringToV8String(isolate, "module not found => " + specifier_string));
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
	log::trace(log::Message("slim::module::resolver::resolve_module_path()",std::string("ends => " + specifier_string_url).c_str(), __FILE__, __LINE__));
}
static void slim::module::resolver::cache_import_specifier(std::shared_ptr<import_specifier> module_import_specifier) {
	by_specifier_cache[module_import_specifier->get_specifier()] = module_import_specifier;
	by_hash_id_cache[module_import_specifier->get_module()->GetIdentityHash()] = module_import_specifier;
}