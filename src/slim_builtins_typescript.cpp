#include <filesystem>
#include <v8.h>
#include <slim/builtins/typescript.h>
#include <slim/common/fetch_and_apply_macros.h>
#include <slim/gv8.h>
#include <slim/module_resolver.h>
using namespace slim::common;
using namespace slim::utilities;
namespace slim::builtins::typescript {
	static int hash_id;
}
int slim::builtins::typescript::get_specifier_module_hash_id() {
	return hash_id;
}
void slim::builtins::typescript::initialize(v8::Isolate* isolate) {
	log::trace(log::Message("slim::builtins::typescript::initialize()","begins",__FILE__, __LINE__));
	auto context = isolate->GetCurrentContext();
	auto specifier_string = "slim_typescript.mjs";
	v8::TryCatch try_catch(isolate);
	auto typescript_import_specifier = slim::module::resolver::resolve_imports(specifier_string, context, true);
	typescript_import_specifier->get_module()->Evaluate(context);
	hash_id = typescript_import_specifier->get_hash_id();
	if(try_catch.HasCaught()) {
		log::error(log::Message("slim::builtins::typescript::initialize","try_catch.HasCaught()",__FILE__, __LINE__));
		slim::gv8::ReportException(&try_catch);
	}
	log::trace(log::Message("slim::builtins::typescript::initialize()","ends",__FILE__, __LINE__));
}
