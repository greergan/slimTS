#ifndef __SLIM__V8__HPP
#define __SLIM__V8__HPP
#include <assert.h>
#include <sstream>
#include <v8.h>
#include <libplatform/libplatform.h>
#include <console.hpp>
#include <utilities.hpp>
namespace slim::veight {
    v8::Local<v8::Script> CompileScript(std::string source, std::string name);
    void CreateGlobal(void);
    v8::Local<v8::Context> GetNewContext(void);
    void ReportException(v8::TryCatch* try_catch);
    bool RunScript(v8::Local<v8::Script> script);
    void init(int argc, char* argv[]);
    void start(void);
    void stop(void);
    v8::Isolate* isolate;
    v8::Local<v8::ObjectTemplate> global;
    v8::ExtensionConfiguration* extensions;
    std::unique_ptr<v8::Platform> platform;
    v8::Isolate::CreateParams create_params;
    void CreateGlobal() {
        global = v8::ObjectTemplate::New(isolate);
    }
    v8::Local<v8::Script> CompileScript(std::string source, std::string name) {
        v8::ScriptOrigin origin(isolate, slim::utilities::StringToValue(isolate, name));
        v8::MaybeLocal<v8::Script> script = v8::Script::Compile(isolate->GetCurrentContext(), slim::utilities::StringToString(isolate, source), &origin);
        return script.ToLocalChecked();
    }
    v8::Local<v8::Context> GetNewContext() {
        return v8::Context::New(isolate, NULL, global);
    }
    void init(int argc, char* argv[]) {
        v8::V8::InitializeICUDefaultLocation(argv[0]);
        v8::V8::InitializeExternalStartupData(argv[0]);
        platform = v8::platform::NewDefaultPlatform();
        v8::V8::InitializePlatform(platform.get());
        v8::V8::Initialize();
        v8::V8::SetFlagsFromCommandLine(&argc, argv, true);
        create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
        isolate = v8::Isolate::New(create_params);
    }
    void ReportException(v8::TryCatch* try_catch) {
        v8::Local<v8::Message> message = try_catch->Message();
        std::string exception_string = slim::utilities::StringValue(isolate, try_catch->Exception());
        if(message.IsEmpty()) {
            //slim::console::error(exception_string);
        }
        else {
            std::string script_line = slim::utilities::ScriptLine(isolate, message);
            std::string script_file_name = slim::utilities::ScriptFileName(isolate, message);
            int script_line_number = slim::utilities::ScriptLineNumber(isolate, message);
            std::string stack_trace = slim::utilities::ScriptStackTrace(isolate, try_catch);
            int end_column = message->GetEndColumn(isolate->GetCurrentContext()).FromJust();
            int start_column = message->GetStartColumn(isolate->GetCurrentContext()).FromJust();
            std::ostringstream messagestream;
            messagestream << script_file_name << ":" << script_line_number << ": " << exception_string << "\n";
            messagestream << script_line << "\n";
            for(int i = 0; i < start_column; i++) {
                messagestream << " ";
            }
            for(int i = start_column; i < end_column; i++) {
                messagestream << "^";
            }
            messagestream << "\n";
            if(stack_trace.length() > 0) {
                messagestream << stack_trace << "\n";
            }
            std::cerr << messagestream.str();
        }
    }
    bool RunScript(v8::Local<v8::Script> script) {
        v8::TryCatch try_catch(isolate);
        v8::Local<v8::Value> result = script->Run(isolate->GetCurrentContext()).ToLocalChecked();
        if(try_catch.HasCaught()) {
            ReportException(&try_catch);
            return false;
        }
        std::cout << slim::utilities::StringValue(isolate, result);
        return true;
    }
    void stop() {
        isolate->Dispose();
        v8::V8::Dispose();
        v8::V8::DisposePlatform();
        delete create_params.array_buffer_allocator;
    }
};
#endif