#include <filesystem>
#include <memory>
#include <regex>
#include <array>
#include <set>
#include <vector>
#include <v8.h>
#include <slim/common/exception.h>
#include <slim/common/log.h>
#include <slim/exception_handler.h>
#include <slim/module_resolver.h>
#include <slim/module/import_specifier.h>
#include <slim/plugin/loader.h>
#include <slim/utilities.h>
namespace slim::module::resolver {
	using namespace slim;
	using namespace slim::common;
	static std::set<std::string> plugins_set{"console", "fs", "kafka", "os", "path", "process", "memoryAdaptor"};
	static specifier_cache_by_specifier by_specifier_cache;
	static specifier_cache_by_hash_id by_hash_id_cache;
}

void IterateFixedArray(v8::Isolate* isolate, v8::Local<v8::FixedArray> fixed_array) {
  	for(int i = 0; i < fixed_array->Length(); ++i) {
		v8::Local<v8::ModuleRequest> module_request = fixed_array->Get(isolate->GetCurrentContext(), i).As<v8::ModuleRequest>();
		slim::common::log::info(slim::utilities::v8StringToString(isolate, module_request->GetSpecifier()));
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
						slim::exception_handler::v8_try_catch_handler(&try_catch);
					}
					return v8::MaybeLocal<v8::Value>(True(isolate));
				};
				log::debug(log::Message("slim::module::resolver::module_call_back_resolver()",std::string("loading plugin => " + specifier_name_string).c_str(),__FILE__, __LINE__));
				slim::plugin::loader::load_plugin(isolate, specifier_name_string, true);
				log::debug(log::Message("slim::module::resolver::module_call_back_resolver()",std::string("loaded plugin => " + specifier_name_string).c_str(),__FILE__, __LINE__));
				if(try_catch.HasCaught()) {
					auto message = slim::utilities::v8StringToString(isolate, try_catch.Message()->Get());
					log::error(log::Message("slim::module::resolver::module_call_back_resolver()", std::string("try_catch.HasCaught() => " + message).c_str(),__FILE__, __LINE__));
					//isolate->ThrowError(try_catch.Message()->Get());
					slim::exception_handler::v8_try_catch_handler(&try_catch);
				}
				const v8::Local<v8::String> v8_default_string = utilities::StringToV8String(isolate, "default");
				std::vector<v8::Local<v8::String>> v8_string_exports_vector;
				v8_string_exports_vector.push_back(v8_default_string);
				utilities::V8KeysToVector(isolate, v8_string_exports_vector, utilities::GetObject(isolate, specifier_name_string, context->Global()));
				context->SetEmbedderData(0, v8_specifier_name); //needed in create_SyntheticModuleEvaluationSteps
				const v8::MemorySpan<const v8::Local<v8::String>> memory_span(v8_string_exports_vector.data(), v8_string_exports_vector.size());
				log::debug(log::Message("slim::module::resolver::module_call_back_resolver()", "setting synthetic plugin", __FILE__, __LINE__));
				import_specifier module_specifier(isolate, specifier_name_string, v8::Module::CreateSyntheticModule(isolate,
									utilities::StringToV8String(isolate, specifier_name_string), memory_span, create_SyntheticModuleEvaluationSteps));
				cache_import_specifier(std::make_shared<import_specifier>(module_specifier));
				current_module_hash_id = module_specifier.get_hash_id();
				log::debug(log::Message("slim::module::resolver::module_call_back_resolver()", "done setting synthetic plugin", __FILE__, __LINE__));
				if(try_catch.HasCaught()) {
					log::error(log::Message("slim::module::resolver::module_call_back_resolver()", "try_catch.HasCaught()",__FILE__, __LINE__));
					slim::exception_handler::v8_try_catch_handler(&try_catch);
				}
			}
		}
		else {
			import_specifier module_specifier(isolate, specifier_name_string, false, referrer);
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
		slim::exception_handler::v8_try_catch_handler(&try_catch);
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
std::shared_ptr<slim::module::import_specifier> slim::module::resolver::resolve_imports(v8::Isolate* isolate,
			std::string entry_script_file_name_string_in, const bool is_entry_point_value = false) {
	log::trace(log::Message("slim::module::resolver::resolve_imports()",std::string("begins => " + entry_script_file_name_string_in).c_str(), __FILE__, __LINE__));
	auto context = isolate->GetCurrentContext();
	import_specifier entry_script_specifier(isolate, entry_script_file_name_string_in, is_entry_point_value, v8::Local<v8::Module>());
	entry_script_specifier.compile_module(); // compile module so we can get at the hash id during module imports
	cache_import_specifier(std::make_shared<import_specifier>(entry_script_specifier)); // now cache it before instantiate_module
	entry_script_specifier.instantiate_module(); // instantiate_module causes import recursion where we need hash id to get at the parent path of current import
	log::trace(log::Message("slim::module::resolver::resolve_imports()",std::string("ends => " + entry_script_specifier.get_specifier_url()).c_str(), __FILE__, __LINE__));
	return by_specifier_cache[entry_script_specifier.get_specifier()];
}
void slim::module::resolver::cache_import_specifier(std::shared_ptr<import_specifier> module_import_specifier) {
	by_specifier_cache[module_import_specifier->get_specifier()] = module_import_specifier;
	by_hash_id_cache[module_import_specifier->get_module()->GetIdentityHash()] = module_import_specifier;
}
std::shared_ptr<slim::module::import_specifier> slim::module::resolver::get_import_specifier_by_hash_id(int hash_id) {
	log::trace(log::Message("slim::module::resolver::get_import_specifier_by_hash_id()",
		std::string("begins => " + std::to_string(hash_id)).c_str(), __FILE__, __LINE__));
	if(by_hash_id_cache.contains(hash_id)) {
		return by_hash_id_cache[hash_id];
	}
	return std::make_shared<slim::module::import_specifier>(slim::module::import_specifier());
	log::trace(log::Message("slim::module::resolver::get_import_specifier_by_hash_id()",
		std::string("ends => " + std::to_string(hash_id)).c_str(), __FILE__, __LINE__));
}
std::shared_ptr<slim::module::import_specifier> slim::module::resolver::get_import_specifier_by_specifier_string(std::string specifier_string) {
	log::trace(log::Message("slim::module::resolver::get_import_specifier_by_specifier_string()",
		std::string("begins => " + specifier_string).c_str(), __FILE__, __LINE__));
	if(by_specifier_cache.contains(specifier_string)) {
		return by_specifier_cache[specifier_string];
	}
	return std::make_shared<slim::module::import_specifier>(slim::module::import_specifier());
	log::trace(log::Message("slim::module::resolver::get_import_specifier_by_specifier_string()",
		std::string("ends => " + specifier_string).c_str(), __FILE__, __LINE__));
}