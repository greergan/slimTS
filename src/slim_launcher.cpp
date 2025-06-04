#include <vector>
#include <v8.h>
#include <libplatform/libplatform.h>
#include <slim/builtins/dummy_console_provider.h>
#include <slim/builtins/typescript.h>
#include <slim/common/log.h>
#include <slim/common/memory_mapper.h>
#include <slim/launcher.h>
#include <slim/module_resolver.h>
namespace slim::launcher {
	using namespace slim::common;
	std::unique_ptr<v8::Platform> platform;
	v8::Isolate::CreateParams create_params;
	bool v8_is_initialized = false;
}
void slim::launcher::initialize(std::vector<std::string> v8_configuration_values) {
	log::trace(log::Message("slim::launcher::initialize()","begins",__FILE__, __LINE__));
	v8::V8::InitializeICUDefaultLocation(v8_configuration_values[0].c_str());
	platform = v8::platform::NewDefaultPlatform();
	v8::V8::InitializePlatform(platform.get());
	v8::V8::Initialize();
	//static void SetFlagsFromString(const char* str);
	//v8::V8::SetFlagsFromCommandLine(&argc, argv, true);
	create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
	v8_is_initialized = true;
	log::trace(log::Message("slim::launcher::initialize()","ends",__FILE__, __LINE__));
}
void slim::launcher::launch(std::string script_name_string) {
	log::trace(log::Message("slim::launcher::launch()","begins",__FILE__, __LINE__));
	auto* isolate = v8::Isolate::New(create_params);
	log::debug(log::Message("slim::launcher::launch()","created new isolate",__FILE__, __LINE__));
	v8::Isolate::Scope isolate_scope(isolate);
	log::debug(log::Message("slim::launcher::launch()","created isolate scope",__FILE__, __LINE__));
    v8::HandleScope handle_scope(isolate);
 	log::debug(log::Message("slim::launcher::launch()","created handle scope",__FILE__, __LINE__));
	auto global_template = v8::ObjectTemplate::New(isolate);
	log::debug(log::Message("slim::launcher::launch()","created global_template",__FILE__, __LINE__));
	//v8::Persistent<v8::ObjectTemplate> persistent_global_template(isolate, global_template);

		auto no_content = [](const v8::FunctionCallbackInfo<v8::Value>& args){};
		global_template->Set(isolate, "setTimeout", v8::FunctionTemplate::New(isolate, no_content));
		log::debug(log::Message("slim::launcher::launch()","created setTimeout stub on the global_template",__FILE__, __LINE__));
		global_template->Set(isolate, "clearTimeout", v8::FunctionTemplate::New(isolate, no_content));
		log::debug(log::Message("slim::launcher::launch()","created clearTimeout stub on the global_template",__FILE__, __LINE__));
		auto context = v8::Context::New(isolate, NULL, global_template);
		log::debug(log::Message("slim::launcher::launch()","created context",__FILE__, __LINE__));
		v8::Context::Scope context_scope(context);
		//slim::dummy_console::expose_plugin(isolate);
		//log::debug(log::Message("slim::launcher::launch()","exposed slim::dummy_console",__FILE__, __LINE__));
		slim::builtins::typescript::initialize(isolate);
		log::debug(log::Message("slim::launcher::launch()","initialized typescript builtin",__FILE__, __LINE__));
		memory_mapper::create("directories");
		log::debug(log::Message("slim::launcher::launch()","created directories memory_map",__FILE__, __LINE__));
	
	log::debug(log::Message("slim::launcher::launch()","created context scope",__FILE__, __LINE__));
	bool is_entry_point = true;
	auto module_import_specifier_ptr = slim::module::resolver::resolve_imports(isolate, script_name_string, is_entry_point);
	log::debug(log::Message("slim::launcher::launch()", std::string("get_module_status_string() => " + module_import_specifier_ptr->get_module_status_string()).c_str(),__FILE__, __LINE__));

	if(module_import_specifier_ptr->get_module()->GetStatus() == v8::Module::Status::kErrored) {
		isolate->ThrowException(module_import_specifier_ptr->get_module()->GetException());
	}
	else {
		auto result = module_import_specifier_ptr->get_module()->Evaluate(context);
		switch(module_import_specifier_ptr->get_module()->GetStatus()) {
			case v8::Module::Status::kErrored:
				isolate->ThrowException(module_import_specifier_ptr->get_module()->GetException());
				break;
			default:
				log::debug(log::Message("slim::launcher::launch()", "checking for stalled top level await modules",__FILE__, __LINE__));
				auto await_message_module_pair = module_import_specifier_ptr->get_module()->GetStalledTopLevelAwaitMessages(isolate);
				auto number_of_stalled_to_level_await_modules = await_message_module_pair.first.size();
				if(number_of_stalled_to_level_await_modules > 0) {
					log::debug(log::Message("slim::launcher::launch()", std::string("number of stalled top level await modules => " + std::to_string(number_of_stalled_to_level_await_modules)).c_str(),__FILE__, __LINE__));
					auto number_of_messages = await_message_module_pair.second.size();
					log::debug(log::Message("slim::launcher::launch()", std::string("number of stalled top level await messages => " + std::to_string(number_of_messages)).c_str(),__FILE__, __LINE__));
				}
				else {
					log::debug(log::Message("slim::launcher::launch()", "no stalled top level await objects",__FILE__, __LINE__));
				}
				log::debug(log::Message("slim::launcher::launch()", std::string("module status after evaluation => " + module_import_specifier_ptr->get_module_status_string()).c_str(),__FILE__, __LINE__));
		}

	}
	log::trace(log::Message("slim::launcher::launch()","ends",__FILE__, __LINE__));
}
void slim::launcher::tear_down() {
	log::trace(log::Message("slim::launcher::tear_down()","begins",__FILE__, __LINE__));
	if(v8_is_initialized) {
		v8::V8::Dispose();
		log::debug(log::Message("slim::launcher::tear_down()","disposed V8",__FILE__, __LINE__));
		v8::V8::DisposePlatform();
		log::debug(log::Message("slim::launcher::tear_down()","disposed platform",__FILE__, __LINE__));
		delete create_params.array_buffer_allocator;
		log::debug(log::Message("slim::launcher::tear_down()","deleted array_buffer_allocator",__FILE__, __LINE__));
		v8_is_initialized = false;
	}
	log::trace(log::Message("slim::launcher::tear_down()","ends",__FILE__, __LINE__));
}