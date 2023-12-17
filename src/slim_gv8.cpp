#include <sstream>
#include <v8.h>
#include <slim/gv8.h>
#include <libplatform/libplatform.h>
#include <slim/utilities.h>
namespace slim::gv8 {
	Gv8Config slim_v8;
}
void slim::gv8::CreateGlobalTemplate() {
	slim_v8.global = v8::ObjectTemplate::New(slim_v8.isolate);
}
v8::Isolate* slim::gv8::GetIsolate() {
	return slim_v8.isolate;
}
v8::Local<v8::Script> slim::gv8::CompileScript(std::string source, std::string name) {
	v8::ScriptOrigin origin(slim_v8.isolate, slim::utilities::StringToValue(slim_v8.isolate, name));
	v8::TryCatch try_catch(slim_v8.isolate);
	v8::MaybeLocal<v8::Script> script = v8::Script::Compile(slim_v8.isolate->GetCurrentContext(), slim::utilities::StringToString(slim_v8.isolate, source), &origin);
	if(try_catch.HasCaught()) {
		ReportException(&try_catch);
	}
	return script.ToLocalChecked();
}
v8::Local<v8::Context> slim::gv8::GetNewContext() {
	return v8::Context::New(slim_v8.isolate, NULL, slim_v8.global);
}
void slim::gv8::initialize(int argc, char* argv[]) {
	v8::V8::InitializeICUDefaultLocation(argv[0]);
	v8::V8::InitializeExternalStartupData(argv[0]);
	slim_v8.platform = v8::platform::NewDefaultPlatform();
	v8::V8::InitializePlatform(slim_v8.platform.get());
	v8::V8::Initialize();
	v8::V8::SetFlagsFromCommandLine(&argc, argv, true);
	slim_v8.create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
	slim_v8.isolate = v8::Isolate::New(slim_v8.create_params);
	slim_v8.initialized = true;
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
			slim_v8.isolate->Dispose();
		}
		v8::V8::Dispose();
		v8::V8::DisposePlatform();
		delete slim_v8.create_params.array_buffer_allocator;
		slim_v8.initialized = false;
	}
}