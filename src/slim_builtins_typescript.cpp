#include <filesystem>
#include <v8.h>
#include <slim/builtins/typescript.h>
#include <slim/common/fetch_and_apply_macros.h>
#include <slim/gv8.h>
#include <slim/common/memory_mapper.h>
#include <slim/module_resolver.h>
using namespace slim::common;
namespace slim::builtins::typescript {
	static int hash_id;
	extern slim::common::memory_mapper::map_container typescript_map_container;
	static memory_mapper::pipe_map_pointer pipe_map;
}
int slim::builtins::typescript::get_specifier_module_hash_id() {
	return hash_id;
}
std::shared_ptr<std::string> slim::builtins::typescript::get_file_content_pointer(std::string file_name_string) {
	log::trace(log::Message("slim::builtins::typescript::get_file_content_pointer()",std::string("begins => " + file_name_string).c_str(),__FILE__, __LINE__));
	if(typescript_map_container.contains(file_name_string)) {
		return typescript_map_container[file_name_string];
	}
	log::trace(log::Message("slim::builtins::typescript::get_file_content_pointer()",std::string("ends => " + file_name_string).c_str(),__FILE__, __LINE__));
	return std::make_shared<std::string>();
}
void slim::builtins::typescript::initialize(v8::Isolate* isolate) {
	log::trace(log::Message("slim::builtins::typescript::initialize()", "begins" ,__FILE__, __LINE__));
	pipe_map = std::make_shared<memory_mapper::map_container>(typescript_map_container);
	memory_mapper::attach("typescript_chain", pipe_map);
	auto context = isolate->GetCurrentContext();
	auto specifier_string = "slim_typescript.mjs";
	v8::TryCatch try_catch(isolate);
	auto typescript_import_specifier = slim::module::resolver::resolve_imports(specifier_string, context, true);
	hash_id = typescript_import_specifier.get()->get_hash_id();
	log::debug(log::Message("slim::builtins::typescript::initialize() hash_id => ", typescript_import_specifier.get()->get_module_status_string().c_str() ,__FILE__, __LINE__));
	typescript_import_specifier->get_module()->Evaluate(context);
	log::debug(log::Message("slim::builtins::typescript::initialize()", "in the middle 2" ,__FILE__, __LINE__));
	if(try_catch.HasCaught()) {
		log::error(log::Message("slim::builtins::typescript::initialize","try_catch.HasCaught()",__FILE__, __LINE__));
		slim::gv8::ReportException(&try_catch);
	}
	log::trace(log::Message("slim::builtins::typescript::initialize()", "ends" ,__FILE__, __LINE__));
}
