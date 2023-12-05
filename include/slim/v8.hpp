#ifndef __SLIM__V8__HPP
#define __SLIM__V8__HPP
#include <sstream>
#include <v8.h>
#include <libplatform/libplatform.h>
#include <slim/utilities.hpp>
namespace slim::gv8 {
    struct V8 {
        bool initialized = false;
        v8::Isolate* isolate;
        v8::Local<v8::ObjectTemplate> global;
        v8::ExtensionConfiguration* extensions;
        std::unique_ptr<v8::Platform> platform;
        v8::Isolate::CreateParams create_params;
    } slim_v8;
    v8::Local<v8::Script> CompileScript(std::string source, std::string name);
    void CreateGlobal(void);
    v8::Isolate* GetIsolate();
    v8::Local<v8::Context> GetNewContext(void);
    void ReportException(v8::TryCatch* try_catch);
    bool RunScript(v8::Local<v8::Script> script);
    void init(int argc, char* argv[]);
    void stop(void);
    void CreateGlobal() {
        slim_v8.global = v8::ObjectTemplate::New(slim_v8.isolate);
    }
    v8::Isolate* GetIsolate() {
        return slim_v8.isolate;
    }
    v8::Local<v8::Script> CompileScript(std::string source, std::string name) {
        v8::ScriptOrigin origin(slim_v8.isolate, slim::utilities::StringToValue(slim_v8.isolate, name));
        v8::TryCatch try_catch(slim_v8.isolate);
        v8::MaybeLocal<v8::Script> script = v8::Script::Compile(slim_v8.isolate->GetCurrentContext(), slim::utilities::StringToString(slim_v8.isolate, source), &origin);
        if(try_catch.HasCaught()) {
            ReportException(&try_catch);
        }
        return script.ToLocalChecked();
    }
    v8::Local<v8::Context> GetNewContext() {
        return v8::Context::New(slim_v8.isolate, NULL, slim_v8.global);
    }
    void init(int argc, char* argv[]) {
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
            v8::TryCatch try_catch(slim_v8.isolate);
            v8::MaybeLocal<v8::Value> result = script->Run(slim_v8.isolate->GetCurrentContext());
            if(try_catch.HasCaught()) {
                ReportException(&try_catch);
            }
        }
        return true;
    }
    void stop() {
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
};
#endif