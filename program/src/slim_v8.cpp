#include <sstream>
#include <utilities.h>
#include <slim_v8.h>
namespace slim::veight {
    static bool initilaized = false;
    static v8::Isolate* isolate;
    static v8::Local<v8::ObjectTemplate> global;
    static v8::ExtensionConfiguration* extensions;
    static std::unique_ptr<v8::Platform> platform;
    static v8::Isolate::CreateParams create_params;
}
void slim::veight::CreateGlobal() {
    global = v8::ObjectTemplate::New(isolate);
}
v8::Isolate* slim::veight::GetIsolate() {
    return isolate;
}
v8::Local<v8::Script> slim::veight::CompileScript(std::string source, std::string name) {
    v8::ScriptOrigin origin(isolate, slim::utilities::StringToValue(isolate, name));
    v8::TryCatch try_catch(isolate);
    auto script = v8::Script::Compile(isolate->GetCurrentContext(), slim::utilities::StringToString(isolate, source), &origin);
    if(try_catch.HasCaught()) {
        ReportException(&try_catch);
    }
    return script.ToLocalChecked();
}
v8::Local<v8::Context> slim::veight::GetNewContext() {
    return v8::Context::New(isolate, NULL, global);
}
void slim::veight::Init(int argc, char* argv[]) {
    v8::V8::InitializeICUDefaultLocation(argv[0]);
    v8::V8::InitializeExternalStartupData(argv[0]);
    platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(platform.get());
    v8::V8::Initialize();
    v8::V8::SetFlagsFromCommandLine(&argc, argv, true);
    create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    isolate = v8::Isolate::New(create_params);
    initilaized = true;
}
void slim::veight::ReportException(v8::TryCatch* try_catch) {
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
bool slim::veight::RunScript(v8::Local<v8::Script> script) {
    if(!script.IsEmpty()) {
        v8::TryCatch try_catch(isolate);
        auto result = script->Run(isolate->GetCurrentContext());
        if(try_catch.HasCaught()) {
            ReportException(&try_catch);
        }
    }
    return true;
}
void slim::veight::Stop() {
    if(initilaized) {
        if(isolate != NULL) {
            isolate->Dispose();
        }
        v8::V8::Dispose();
        v8::V8::DisposePlatform();
        delete create_params.array_buffer_allocator;
        initilaized = false;
    }
}