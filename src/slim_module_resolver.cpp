#include <filesystem>
#include <regex>
#include <set>
#include <v8.h>
#include <slim/common/exception.h>
#include <slim/common/log.h>
#include <slim/common/fetch_and_apply_macros.h>
#include <slim/gv8.h>
#include <slim/module_resolver.h>
#include <slim/plugin/loader.h>
#include <slim/utilities.h>
namespace slim::module::resolver {
	using namespace slim::utilities;
	using namespace slim::common::log;
	static std::set<std::string> plugins_set{"console", "fs", "kafka", "os", "path", "process"};
	specifier_cache module_specifier_cache;
}
slim::module::resolver::import_specifier::import_specifier() {}
slim::module::resolver::import_specifier::import_specifier(std::string_view specifier_string, v8::Local<v8::Context>& context,
			v8::Local<v8::Module> synthetic_module) : specifier_string(specifier_string), context(context), v8_module(synthetic_module) {
	isolate = context->GetIsolate();
	instantiate_module();
	v8_module->Evaluate(context).FromMaybe(v8::Local<v8::Value>());
}
slim::module::resolver::import_specifier::import_specifier(std::string_view specifier_string_view_in,
		v8::Local<v8::Context>& context, const bool is_entry_point_value = false) : context(context), is_entry_point_value(is_entry_point_value) {
	std::filesystem::path specifier_path = std::filesystem::absolute(specifier_string_view_in);
	isolate = context->GetIsolate();
	specifier_string = specifier_path.string();
	specifier_path_string = specifier_path.parent_path().string();
	fetch_source();
	compile_module();
	instantiate_module();
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
const std::string& slim::module::resolver::import_specifier::get_path() const {
	return specifier_path_string;
}
const std::string& slim::module::resolver::import_specifier::get_specifier() const {
	return specifier_string;
}
const bool slim::module::resolver::import_specifier::has_module() const {
	return has_module_value;
}
const bool slim::module::resolver::import_specifier::is_entry_point() const {
	return is_entry_point_value;
}
void slim::module::resolver::import_specifier::compile_module() {
	trace(Message("slim::module::resolver::import_specifier::compile_module()",std::string("begins     =>" + specifier_string).c_str(),__FILE__, __LINE__));
	v8::TryCatch try_catch(isolate);
	v8::ScriptOrigin origin(StringToValue(isolate, specifier_string), 0, 0, false, -1, StringToValue(isolate, ""), false, false, true);
	v8::ScriptCompiler::Source v8_module_source(StringToString(isolate, specifier_source_code), origin);
	v8::ScriptCompiler::CompileOptions module_compile_options(v8::ScriptCompiler::kNoCompileOptions);
	v8::ScriptCompiler::NoCacheReason module_no_cache_reason(v8::ScriptCompiler::kNoCacheNoReason);
	v8::MaybeLocal<v8::Module> temporary_module = v8::ScriptCompiler::CompileModule(isolate, &v8_module_source, module_compile_options, module_no_cache_reason);
	if(!temporary_module.IsEmpty()) {
		v8_module = temporary_module.ToLocalChecked();
		has_module_value = true;
	}
	if(try_catch.HasCaught()) {
		error(Message("slim::module::resolver::import_specifier::compile_module()", "try_catch.HasCaught()",__FILE__, __LINE__));
		slim::gv8::ReportException(&try_catch);
	}
	trace(Message("slim::module::resolver::import_specifier::compile_module()", "ends", __FILE__, __LINE__));
}
void slim::module::resolver::import_specifier::fetch_source() {
	trace(Message("slim::module::resolver::import_specifier::fetch_source()",std::string("begins file =>" + specifier_string).c_str(),__FILE__, __LINE__));
	specifier_source_code = slim::common::fetch_and_apply_macros(specifier_string);
	std::regex from_pattern("[[:space:]\n]+from[[:space:]\n]+[\\\"\\'][.](\\/.+[^\"])[\\\"\\']");
	std::string expanded_path_statement =  " from '" + specifier_path_string + "$1'";
	specifier_source_code = std::regex_replace(specifier_source_code, from_pattern, expanded_path_statement);
	debug(Message("slim::module::resolver::import_specifier::fetch_source()",std::string("specifier_string => " + specifier_string).c_str(), __FILE__, __LINE__));
	debug(Message("slim::module::resolver::import_specifier::fetch_source()",std::string("specifier_path_string => " + specifier_path_string).c_str(), __FILE__, __LINE__));
	//debug(Message("slim::module::resolver::import_specifier::fetch_source()",std::string("source_string => " + specifier_source_code).c_str(), __FILE__, __LINE__));
	trace(Message("slim::module::resolver::import_specifier::fetch_source()", "ends" ,__FILE__, __LINE__));
}
void slim::module::resolver::import_specifier::instantiate_module() {
	trace(Message("slim::module::resolver::import_specifier::instantiate_module()",std::string("begins =>" + specifier_string).c_str(),__FILE__, __LINE__));
	v8::TryCatch try_catch(isolate);
	auto result = v8_module->InstantiateModule(context, module_call_back_resolver);
	debug(Message("slim::module::resolver::import_specifier::instantiate_module()",
		std::string("v8_module->InstantiateModule() status => " + get_module_status_string()).c_str(),__FILE__, __LINE__));
	if(try_catch.HasCaught()) {
		error(Message("slim::module::resolver::import_specifier::instantiate_module()", "try_catch.HasCaught()",__FILE__, __LINE__));
		slim::gv8::ReportException(&try_catch);
	}
	trace(Message("slim::module::resolver::import_specifier::instantiate_module()","ends",__FILE__, __LINE__));
}
v8::MaybeLocal<v8::Module> slim::module::resolver::module_call_back_resolver(
		v8::Local<v8::Context> context, v8::Local<v8::String> v8_specifier_name, v8::Local<v8::FixedArray> import_assertions, v8::Local<v8::Module> referrer) {
	trace(Message("slim::module::resolver::module_call_back_resolver()","begins", __FILE__, __LINE__));
	auto isolate = context->GetIsolate();
	std::string specifier_name_string = v8StringToString(isolate, v8_specifier_name);
	if(module_specifier_cache[specifier_name_string].has_module()) {
		return module_specifier_cache[specifier_name_string].get_module();
	}
	std::string source_file_contents;
	debug(Message("slim::module::resolver::module_call_back_resolver()",std::string("loading => " + specifier_name_string).c_str(),__FILE__, __LINE__));
	try {
		if(plugins_set.contains(specifier_name_string)) {
			auto create_SyntheticModuleEvaluationSteps = [](v8::Local<v8::Context> context, v8::Local<v8::Module> module) -> v8::MaybeLocal<v8::Value> {
				auto isolate = context->GetIsolate();
				auto plugin_name_string = v8ValueToString(isolate, context->GetEmbedderData(0));
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
				return v8::MaybeLocal<v8::Value>(True(isolate));
			};
			debug(Message("slim::module::resolver::module_call_back_resolver()",std::string("loading plugin => " + specifier_name_string).c_str(),__FILE__, __LINE__));
			slim::plugin::loader::load_plugin(isolate, specifier_name_string, true);
			debug(Message("slim::module::resolver::module_call_back_resolver()",std::string("loaded plugin => " + specifier_name_string).c_str(),__FILE__, __LINE__));
			const v8::Local<v8::String> v8_default_string = StringToV8String(isolate, "default");
			std::vector<v8::Local<v8::String>> v8_string_exports_vector;
			v8_string_exports_vector.push_back(v8_default_string);
			V8KeysToVector(isolate, v8_string_exports_vector, GetObject(isolate, specifier_name_string, context->Global()));
			context->SetEmbedderData(0, v8_specifier_name);
			const v8::MemorySpan<const v8::Local<v8::String>> memory_span(v8_string_exports_vector.data(), v8_string_exports_vector.size());
			debug(Message("slim::module::resolver::module_call_back_resolver()", "setting synthetic plugin", __FILE__, __LINE__));
			import_specifier entry_script_specifier(specifier_name_string, context, v8::Module::CreateSyntheticModule(isolate,
				StringToV8String(isolate, specifier_name_string), memory_span, create_SyntheticModuleEvaluationSteps));
			module_specifier_cache[specifier_name_string] = entry_script_specifier;
			debug(Message("slim::module::resolver::module_call_back_resolver()", "done setting synthetic plugin", __FILE__, __LINE__));
		}
		else {
			debug(Message("slim::module::resolver::module_call_back_resolver()", std::string("loading file => " + specifier_name_string).c_str(),__FILE__, __LINE__));
			bool is_entry_point = false;
			import_specifier entry_script_specifier(specifier_name_string, context, is_entry_point);
			module_specifier_cache[specifier_name_string] = entry_script_specifier;
		}
	}
    catch(const slim::common::SlimFileException& _error) {
        std::string error_message = _error.message + ", path => " + _error.path;
        error(Message(_error.call.c_str(), error_message.c_str(),__FILE__, __LINE__));
		isolate->ThrowError(StringToV8String(isolate, "Module not found: " + specifier_name_string));
    }
	trace(Message("slim::module::resolver::module_call_back_resolver()",std::string("ends =>" + specifier_name_string).c_str(), __FILE__, __LINE__));
	return module_specifier_cache[specifier_name_string].get_module();
}
slim::module::resolver::specifier_cache& slim::module::resolver::resolve_imports(
						std::string entry_script_file_name_string_in, v8::Local<v8::Context> context, const bool is_entry_point_value) {
	trace(Message("slim::module::resolver::resolve_imports()","begins", __FILE__, __LINE__));
	import_specifier entry_script_specifier(entry_script_file_name_string_in, context, is_entry_point_value);
	module_specifier_cache[entry_script_file_name_string_in] = entry_script_specifier;
	trace(Message("slim::module::resolver::resolve_imports()","ends", __FILE__, __LINE__));
	return module_specifier_cache;
}