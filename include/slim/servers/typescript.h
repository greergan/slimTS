#ifndef __SLIM__BUILTINS__TYPESCRIPT__H
#define __SLIM__BUILTINS__TYPESCRIPT__H
#include <functional>
#include <memory>
#include <string>
#include <v8.h>
#include <slim/common/memory_mapper.h>
#include <slim/common/log.h>
#include <slim/utilities.h>
namespace slim::builtins::typescript {
	static const std::string memory_mapper_pipe_name_string{"typescript_chain"};
	static const std::string typescript_executable_file_url{"file:///bin/slim_typescript.mjs"};
	using v8_function_call_back_type = std::function<void(const v8::FunctionCallbackInfo<v8::Value>&)>;
	void compile(v8::Isolate* isolate, std::string specifier_string_url, v8_function_call_back_type& call_back);
	std::string get_executable_script();
	std::shared_ptr<std::string> get_file_content_pointer(std::string file_name_string);
	const std::string& get_memory_mapper_pipe_handle();
	int get_specifier_module_hash_id();
	void initialize(v8::Isolate* isolate);
	bool is_typescript_executable(const std::string& typescript_executable_candidate);
	void store_file_content(std::string file_name_string, std::shared_ptr<std::string> file_data_string);
	using namespace slim;
	using namespace slim::common;
	extern v8::Global<v8::Context> typescript_only_global_context;
	extern v8::Local<v8::Function> typescript_compile_function;
	template<typename Function, typename Signature = typename std::decay<Function>::type>
	void compile(v8::Isolate* isolate, std::string specifier_string_url, Function&& call_back) {
		log::trace(log::Message("slim::builtins::typescript::compile()", std::string("begins => " + specifier_string_url).c_str(),__FILE__, __LINE__));
		auto context = typescript_only_global_context.Get(isolate);
		auto call_back_function_template = v8::FunctionTemplate::New(isolate, std::forward<Signature>(call_back));
		log::debug(log::Message("slim::builtins::typescript::compile()", "created call_back_function_template",__FILE__, __LINE__));
		auto call_back_function_maybe = call_back_function_template->GetFunction(context);
		if(call_back_function_maybe.IsEmpty()) {
			isolate->ThrowException(utilities::StringToV8Value(isolate, "typescript compile function is null"));
		}
		else {
			auto call_back_function = call_back_function_maybe.ToLocalChecked();
			log::debug(log::Message("slim::builtins::typescript::compile()", "created function",__FILE__, __LINE__));
			const int argc = 2;
			v8::Local<v8::Value> args[argc] = {utilities::StringToV8String(isolate, specifier_string_url), call_back_function};
			log::debug(log::Message("slim::builtins::typescript::compile()", "loaded function arguments array",__FILE__, __LINE__));
			auto call_result = typescript_compile_function->Call(context, context->Global(), argc, args).ToLocalChecked();
			log::debug(log::Message("slim::builtins::typescript::compile()", "called function",__FILE__, __LINE__));
			if(call_result.IsEmpty()) {
				log::trace(log::Message("slim::builtins::typescript::compile()", std::string("error calling function => " + specifier_string_url).c_str(),__FILE__, __LINE__));
			}
			//return call result
		}
		log::trace(log::Message("slim::builtins::typescript::compile()", std::string("ends => " + specifier_string_url).c_str(),__FILE__, __LINE__));
	}
}
#endif