#include <functional>
#include <memory>
#include <v8.h>
#include <slim/builtins/typescript.h>
#include <slim/common/memory_mapper.h>
#include <slim/exception_handler.h>
#include <slim/module_resolver.h>
namespace slim::builtins::typescript {
	using namespace slim;
	using namespace slim::common;
	static int hash_id;
	static bool map_is_attached = false;
	v8::Local<v8::Function> typescript_compile_function;
	v8::Global<v8::Context> typescript_only_global_context;
	extern slim::common::memory_mapper::map_container typescript_map_container;
	static memory_mapper::pipe_map_pointer pipe_map = nullptr;
}
int slim::builtins::typescript::get_specifier_module_hash_id() {
	return hash_id;
}
std::string slim::builtins::typescript::get_executable_script() {
	log::trace(log::Message("slim::builtins::typescript::get_executable_script()", "begins",__FILE__, __LINE__));
	if(!map_is_attached) {
		log::debug(log::Message("slim::builtins::typescript::get_executable_script()", "attaching slot",__FILE__, __LINE__));
		pipe_map = std::make_shared<memory_mapper::map_container>(typescript_map_container);
		memory_mapper::attach(memory_mapper_pipe_name_string, pipe_map);
		map_is_attached = true;
	}
	log::debug(log::Message("slim::builtins::typescript::get_executable_script()", "after attaching map",__FILE__, __LINE__));
	if(memory_mapper::exists(memory_mapper_pipe_name_string) && (memory_mapper::exists(memory_mapper_pipe_name_string, typescript_executable_file_url))) {
		return *memory_mapper::read(memory_mapper_pipe_name_string, typescript_executable_file_url);
	}
	else {
		throw("Typescript executable script not found");
	}
	log::trace(log::Message("slim::builtins::typescript::get_executable_script()", "ends",__FILE__, __LINE__));
	return "unreachable";
}
std::shared_ptr<std::string> slim::builtins::typescript::get_file_content_pointer(std::string file_name_string) {
	log::trace(log::Message("slim::builtins::typescript::get_file_content_pointer()",std::string("begins => " + file_name_string).c_str(),__FILE__, __LINE__));
	if(memory_mapper::exists(memory_mapper_pipe_name_string, file_name_string)) {
		log::debug(log::Message("slim::builtins::typescript::get_file_content_pointer()",std::string("found => " + file_name_string).c_str(),__FILE__, __LINE__));
		return memory_mapper::read(memory_mapper_pipe_name_string, file_name_string);
	}
	log::debug(log::Message("slim::builtins::typescript::get_file_content_pointer()",std::string("did not find => " + file_name_string).c_str(),__FILE__, __LINE__));
	log::trace(log::Message("slim::builtins::typescript::get_file_content_pointer()",std::string("ends => " + file_name_string).c_str(),__FILE__, __LINE__));
	return std::make_shared<std::string>();
}
void slim::builtins::typescript::initialize(v8::Isolate* isolate) {
	log::trace(log::Message("slim::builtins::typescript::initialize()", "begins" ,__FILE__, __LINE__));
	auto context = v8::Context::New(isolate);
	log::debug(log::Message("slim::builtins::typescript::initialize()", "created context in local scope" ,__FILE__, __LINE__));
	if(context.IsEmpty()) {
		isolate->ThrowException(utilities::StringToV8String(isolate, "failed to create context"));
	}
	//v8::Context::Scope scope_handle(context);
	log::debug(log::Message("slim::builtins::typescript::initialize()", "created context scope handle" ,__FILE__, __LINE__));
	pipe_map = std::make_shared<memory_mapper::map_container>(typescript_map_container);
	log::debug(log::Message("slim::builtins::typescript::initialize()", "made shared pointer to typescript_map_container" ,__FILE__, __LINE__));
	memory_mapper::attach(memory_mapper_pipe_name_string, pipe_map);
	v8::TryCatch try_catch(isolate);
	auto typescript_import_specifier = slim::module::resolver::resolve_imports(isolate, typescript_executable_file_url, true);
	hash_id = typescript_import_specifier.get()->get_hash_id();
	log::debug(log::Message("slim::builtins::typescript::initialize() hash_id => ", typescript_import_specifier.get()->get_module_status_string().c_str() ,__FILE__, __LINE__));
	typescript_import_specifier->get_module()->Evaluate(context);
	log::debug(log::Message("slim::builtins::typescript::initialize()", "evaluated typescript_import_specifier module" ,__FILE__, __LINE__));
	auto typescript_module_namespace_object = typescript_import_specifier->get_module()->GetModuleNamespace()->ToObject(context).ToLocalChecked();
	log::debug(log::Message("slim::builtins::typescript::initialize()", "found namespace object",__FILE__, __LINE__));
	v8::Local<v8::Value> v8_compile_function_value;
	if(!typescript_module_namespace_object->Get(context, utilities::StringToV8String(isolate, "compile")).ToLocal(&v8_compile_function_value)) {
		log::error(log::Message("slim::builtins::typescript::initialize()", "Error getting compile function for typescript compiler",__FILE__, __LINE__));
	}
	log::debug(log::Message("slim::builtins::typescript::initialize()", "found  v8_compile_function_value object",__FILE__, __LINE__));
	typescript_compile_function = v8::Local<v8::Function>::Cast(v8_compile_function_value);
	log::debug(log::Message("slim::builtins::typescript::initialize()", "created typescript_compile_function",__FILE__, __LINE__));
	if(typescript_compile_function.IsEmpty()) {
		log::error(log::Message("slim::builtins::typescript::initialize()", "typescript_compile_function.IsEmpty()",__FILE__, __LINE__));
	}
	if(try_catch.HasCaught()) {
		log::error(log::Message("slim::builtins::typescript::initialize","try_catch.HasCaught()",__FILE__, __LINE__));
		slim::exception_handler::v8_try_catch_handler(&try_catch);
	}
	typescript_only_global_context = v8::Global<v8::Context>(isolate, context);
	log::debug(log::Message("slim::builtins::typescript::initialize()", "moved typescript_only_context to global scope" ,__FILE__, __LINE__));
	log::trace(log::Message("slim::builtins::typescript::initialize()", "ends" ,__FILE__, __LINE__));
/* 
		log::trace(log::Message("slim::builtins::typescript::initialize()", "begins" ,__FILE__, __LINE__));
	auto typescript_only_context = v8::Context::New(isolate);
	log::debug(log::Message("slim::builtins::typescript::initialize()", "created typescript_only_context in local scope" ,__FILE__, __LINE__));
	if(typescript_only_context.IsEmpty()) {
		isolate->ThrowException(utilities::StringToV8String(isolate, "failed to create typescript_only_context"));
	}
	v8::Context::Scope scope_handle(typescript_only_context);
	log::debug(log::Message("slim::builtins::typescript::initialize()", "created typescript_only_context scope handle" ,__FILE__, __LINE__));
	pipe_map = std::make_shared<memory_mapper::map_container>(typescript_map_container);
	log::debug(log::Message("slim::builtins::typescript::initialize()", "made shared pointer to typescript_map_container" ,__FILE__, __LINE__));
	memory_mapper::attach("typescript_chain", pipe_map);
	v8::TryCatch try_catch(isolate);
	auto typescript_import_specifier = slim::module::resolver::resolve_imports(isolate, "slim_typescript.mjs", true);
	hash_id = typescript_import_specifier.get()->get_hash_id();
	log::debug(log::Message("slim::builtins::typescript::initialize() hash_id => ", typescript_import_specifier.get()->get_module_status_string().c_str() ,__FILE__, __LINE__));
	typescript_import_specifier->get_module()->Evaluate(typescript_only_context);
	log::debug(log::Message("slim::builtins::typescript::initialize()", "evaluated typescript_import_specifier module" ,__FILE__, __LINE__));
	auto typescript_module_namespace_object = typescript_import_specifier->get_module()->GetModuleNamespace()->ToObject(typescript_only_context).ToLocalChecked();
	log::debug(log::Message("slim::builtins::typescript::initialize()", "found namespace object",__FILE__, __LINE__));
	auto v8_function_name_string = utilities::StringToV8String(isolate, "compile");
	v8::Local<v8::Value> v8_compile_function_value;
	if(!typescript_module_namespace_object->Get(typescript_only_context, v8_function_name_string).ToLocal(&v8_compile_function_value)) {
		log::error(log::Message("slim::builtins::typescript::initialize()", "Error getting compile function for typescript compiler",__FILE__, __LINE__));
	}
	log::debug(log::Message("slim::builtins::typescript::initialize()", "found  v8_compile_function_value object",__FILE__, __LINE__));
	typescript_compile_function = v8::Local<v8::Function>::Cast(v8_compile_function_value);
	log::debug(log::Message("slim::builtins::typescript::initialize()", "created typescript_compile_function",__FILE__, __LINE__));
	if(typescript_compile_function.IsEmpty()) {
		log::error(log::Message("slim::builtins::typescript::initialize()", "typescript_compile_function.IsEmpty()",__FILE__, __LINE__));
	}
	if(try_catch.HasCaught()) {
		log::error(log::Message("slim::builtins::typescript::initialize","try_catch.HasCaught()",__FILE__, __LINE__));
		slim::exception_handler::v8_try_catch_handler(&try_catch);
	}
	typescript_only_global_context = v8::Global<v8::Context>(isolate, typescript_only_context);
	log::debug(log::Message("slim::builtins::typescript::initialize()", "moved typescript_only_context to global scope" ,__FILE__, __LINE__));
	log::trace(log::Message("slim::builtins::typescript::initialize()", "ends" ,__FILE__, __LINE__)); */
}
bool slim::builtins::typescript::is_typescript_executable(const std::string& typescript_executable_candidate) {
	log::trace(log::Message("slim::builtins::typescript::is_typescript_executable()", std::string("begins => " + typescript_executable_candidate).c_str(),__FILE__, __LINE__));
	auto debug_message = typescript_executable_candidate + " == " + typescript_executable_file_url + " => ";
	auto debug_value = typescript_executable_candidate == typescript_executable_file_url ? "true" : "false";
	log::trace(log::Message("slim::builtins::typescript::is_typescript_executable()", std::string(debug_message + debug_value).c_str(),__FILE__, __LINE__));
	log::trace(log::Message("slim::builtins::typescript::is_typescript_executable()", std::string("ends => " + typescript_executable_candidate).c_str(),__FILE__, __LINE__));
	return typescript_executable_candidate == typescript_executable_file_url;
}
void slim::builtins::typescript::store_file_content(std::string file_name_url_string, std::shared_ptr<std::string> file_data_string_pointer) {
	log::trace(log::Message("slim::builtins::typescript::put_file_content()", std::string("begins => " + file_name_url_string).c_str(),__FILE__, __LINE__));
	log::debug(log::Message("slim::builtins::typescript::put_file_content()", std::string("file size before storage => " + std::to_string(file_data_string_pointer->size())).c_str(),__FILE__, __LINE__));
	slim::common::memory_mapper::write(memory_mapper_pipe_name_string, file_name_url_string, file_data_string_pointer);
	//log::debug(log::Message("slim::builtins::typescript::put_file_content()", std::string("file size after storage => " + std::to_string(typescript_map_container[file_name_url_string]->size())).c_str(),__FILE__, __LINE__));
	log::trace(log::Message("slim::builtins::typescript::put_file_content()",std::string("ends => " + file_name_url_string).c_str(),__FILE__, __LINE__));
}
