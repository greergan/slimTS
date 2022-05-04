#include <sstream>
#include <slim_v8.h>
void slim::veight::CreateGlobal() {
    global = v8::ObjectTemplate::New(isolate);
}
v8::Isolate* slim::veight::GetIsolate() {
    return isolate;
}
v8::Local<v8::Script> slim::veight::CompileScript(std::string source, std::string name) {
        v8::ScriptOrigin origin(isolate, slim::utilities::StringToValue(isolate, name));
        v8::TryCatch try_catch(veight.isolate);
        v8::MaybeLocal<v8::Script> script = v8::Script::Compile(veight.isolate->GetCurrentContext(), slim::utilities::StringToString(veight.isolate, source), &origin);
        if(try_catch.HasCaught()) {
            ReportException(&try_catch);
        }
        return script.ToLocalChecked();
    }
    v8::Local<v8::Context> GetNewContext() {
        return v8::Context::New(veight.isolate, NULL, veight.global);
    }
    void init(int argc, char* argv[]) {
        v8::V8::InitializeICUDefaultLocation(argv[0]);
        v8::V8::InitializeExternalStartupData(argv[0]);
        veight.platform = v8::platform::NewDefaultPlatform();
        v8::V8::InitializePlatform(veight.platform.get());
        v8::V8::Initialize();
        v8::V8::SetFlagsFromCommandLine(&argc, argv, true);
        veight.create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
        veight.isolate = v8::Isolate::New(veight.create_params);
        veight.initilaized = true;
    }
    void ReportException(v8::TryCatch* try_catch) {
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
    bool RunScript(v8::Local<v8::Script> script) {
        if(!script.IsEmpty()) {
            v8::TryCatch try_catch(veight.isolate);
            v8::MaybeLocal<v8::Value> result = script->Run(veight.isolate->GetCurrentContext());
            if(try_catch.HasCaught()) {
                ReportException(&try_catch);
            }
        }
        return true;
    }
    void stop() {
        if(veight.initilaized) {
            if(veight.isolate != NULL) {
                veight.isolate->Dispose();
            }
            v8::V8::Dispose();
            v8::V8::DisposePlatform();
            delete veight.create_params.array_buffer_allocator;
            veight.initilaized = false;
        }
    }
};
#endif