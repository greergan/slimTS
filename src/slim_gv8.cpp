#include <iostream>

#include <sstream>
#include <v8.h>
#include <slim/gv8.h>
#include <libplatform/libplatform.h>
#include <slim/utilities.h>
namespace slim::gv8 {
	Gv8Config slim_v8;
}
void slim::gv8::CreateGlobalTemplate() {
	slim_v8.globalObjectTemplate = v8::ObjectTemplate::New(slim_v8.isolate);
}
v8::Local<v8::Module> slim::gv8::CompileAndInstantiateModule(std::string source, std::string name) {
	auto context = slim_v8.isolate->GetCurrentContext();
	v8::TryCatch try_catch(slim_v8.isolate);

	v8::ScriptOrigin origin(slim::utilities::StringToValue(slim_v8.isolate, name),
						0, 0, false, -1, slim::utilities::StringToValue(slim_v8.isolate, ""), false, false, true);
	v8::ScriptCompiler::Source v8_source(slim::utilities::StringToString(slim_v8.isolate, source), origin);
	v8::ScriptCompiler::CompileOptions module_compile_options(v8::ScriptCompiler::kNoCompileOptions);
	v8::ScriptCompiler::NoCacheReason module_no_cache_reason(v8::ScriptCompiler::kNoCacheNoReason);
	auto module = v8::ScriptCompiler::CompileModule(slim_v8.isolate, &v8_source, module_compile_options, module_no_cache_reason).ToLocalChecked();
	if(try_catch.HasCaught()) {
		ReportException(&try_catch);
	}
	if(!module.IsEmpty()) {
		bool instantiated = slim::utilities::V8BoolToBool(slim_v8.isolate, module->InstantiateModule(slim_v8.isolate->GetCurrentContext(), ModuleCallbackResolver));
		if(try_catch.HasCaught()) {
			slim::gv8::ReportException(&try_catch);
		}
		if(!instantiated) {
			slim_v8.isolate->ThrowException(slim::utilities::StringToV8String(slim_v8.isolate, "Initial module instantiation failed: " +  name));
		}
	}
	return module;
}
v8::Local<v8::Script> slim::gv8::CompileScript(std::string source, std::string name) {
	v8::TryCatch try_catch(slim_v8.isolate);
	v8::MaybeLocal<v8::Script> script = v8::Script::Compile(slim_v8.isolate->GetCurrentContext(), slim::utilities::StringToString(slim_v8.isolate, source));
	if(try_catch.HasCaught()) {
		ReportException(&try_catch);
	}
	return script.ToLocalChecked();
}
v8::Local<v8::ObjectTemplate>& slim::gv8::GetGlobalObjectTemplate() {
	return slim_v8.globalObjectTemplate;
}
v8::Isolate* slim::gv8::GetIsolate() {
	return slim_v8.isolate;
}
v8::Local<v8::Context> slim::gv8::GetNewContext() {
	return v8::Context::New(slim_v8.isolate, NULL, slim_v8.globalObjectTemplate);
}
void slim::gv8::initialize(int argc, char* argv[]) {
	v8::V8::InitializeICUDefaultLocation(argv[0]);
	slim_v8.platform = v8::platform::NewDefaultPlatform();
	v8::V8::InitializePlatform(slim_v8.platform.get());
	v8::V8::Initialize();
	v8::V8::SetFlagsFromCommandLine(&argc, argv, true);
	slim_v8.create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
	slim_v8.isolate = v8::Isolate::New(slim_v8.create_params);
	slim_v8.isolate->Enter();
	slim_v8.initialized = true;
}
v8::MaybeLocal<v8::Module> slim::gv8::ModuleCallbackResolver(v8::Local<v8::Context> context, v8::Local<v8::String> specifier, v8::Local<v8::FixedArray> import_assertions, v8::Local<v8::Module> referrer) {
	v8::Isolate* isolate = context->GetIsolate();
	// fetch(specifier)
	std::string file_contents;
	std::string file_name = slim::utilities::v8StringToString(isolate, specifier);
	if(file_name.starts_with("file://")) {
		file_name = file_name.substr(7);
	}
	std::ifstream file(file_name);
	if(file.is_open()) {
		getline(file, file_contents, '\0');
		file.close();
	}
	else {
		isolate->ThrowException(slim::utilities::StringToV8String(isolate, "Module not found: " + file_name));
	}
	return CompileAndInstantiateModule(file_contents, file_name);
}
void slim::gv8::ReportException(v8::TryCatch* try_catch) {
	std::stringstream exception_string;
	auto isolate = try_catch->Message()->GetIsolate();
	auto context = isolate->GetCurrentContext();
	exception_string << slim::utilities::StringValue(isolate, try_catch->Exception()) << "\n";
	exception_string << slim::utilities::StringValue(isolate, try_catch->Message()->GetSourceLine(context).ToLocalChecked()) << "\n";
	for(int i = 0; i < try_catch->Message()->GetStartColumn(); i++) {
		exception_string << " ";
	}
	exception_string << "^" << "\n";
	throw(exception_string.str());
}
bool slim::gv8::RunScript(v8::Local<v8::Script> script) {
	if(!script.IsEmpty()) {
		v8::TryCatch try_catch(slim_v8.isolate);
		v8::MaybeLocal<v8::Value> result = script->Run(slim_v8.isolate->GetCurrentContext());
		if(try_catch.HasCaught()) {
			ReportException(&try_catch);
		}
	}
	return true;
}
void slim::gv8::stop() {
	if(slim_v8.initialized) {
		if(slim_v8.isolate != NULL) {
			slim_v8.isolate->Exit();
			slim_v8.isolate->Dispose();
		}
		v8::V8::Dispose();
		v8::V8::DisposePlatform();
		delete slim_v8.create_params.array_buffer_allocator;
		slim_v8.initialized = false;
	}
}