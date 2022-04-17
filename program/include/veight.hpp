#ifndef __SLIM__V8__HPP
#define __SLIM__V8__HPP
#include <assert.h>
#include <v8.h>
#include <libplatform/libplatform.h>
#include <http_server.hpp>
using namespace v8;
const char* ToCString(const String::Utf8Value& value) {
    return *value ? *value : "<string conversion failed>";
}
void Print(const FunctionCallbackInfo<Value>& args) {
    bool first = true;
    for(int i = 0; i < args.Length(); i++) {
        HandleScope handle_scope(args.GetIsolate());
        if(first) {
            first = false;
        }
        else {
            printf(" ");
        }
        String::Utf8Value str(args.GetIsolate(), args[i]);
        const char* cstr = ToCString(str);
        printf("%s", cstr);
    }
    printf("\n");
    fflush(stdout);
}
void HttpListen(const FunctionCallbackInfo<Value>& args) {
    HandleScope handle_scope(args.GetIsolate());
    slim::http::start();
}

struct Log {
    static void notice() { std::cout << "notice\n"; }
};

namespace slim::veight {
    using namespace v8;
    struct Process {
        private:
            Isolate* isolate;
            Local<ObjectTemplate> global;
            ExtensionConfiguration* extensions;
            std::unique_ptr<Platform> platform;
            Isolate::CreateParams create_params;
            //extensions
            Log* log = new Log();
            void ExposeLog(Local<Context> context) {
                
                Local<ObjectTemplate> log_template = ObjectTemplate::New(this->isolate);
                log_template->SetInternalFieldCount(6);
                //Local<Object> log_object = log_template->NewInstance(context).ToLocalChecked();
                //log_object->SetInternalField(0, External::New(this->isolate, log));
                //log_object->Set(String::NewFromUtf8("notice").ToLocalChecked(), v8::FunctionCallback(log->notice), ReadOnly);
                //this->global->Set(String::NewFromUtf8(this->isolate, "log").ToLocalChecked(), log_object, ReadOnly);
            }
        public:
            void RegisterFunctions() {
                //this->global->Set(String::NewFromUtf8(this->isolate, "version"), String::NewFromUtf8(this->isolate, "0.0"), ReadOnly);
                this->global->Set(String::NewFromUtf8(this->isolate, "http", NewStringType::kNormal).ToLocalChecked(),
                                    FunctionTemplate::New(this->isolate, HttpListen));
                this->global->Set(String::NewFromUtf8(this->isolate, "print", NewStringType::kNormal).ToLocalChecked(),
                                    FunctionTemplate::New(this->isolate, Print));
            }
            Process(int argc, char* argv[]) {
                V8::InitializeICUDefaultLocation(argv[0]);
                V8::InitializeExternalStartupData(argv[0]);
                this->platform = v8::platform::NewDefaultPlatform();
                V8::InitializePlatform(platform.get());
                V8::Initialize();
                V8::SetFlagsFromCommandLine(&argc, argv, true);
                this->create_params.array_buffer_allocator = ArrayBuffer::Allocator::NewDefaultAllocator();
                this->isolate = Isolate::New(this->create_params);
                //this->extensions = new ExtensionConfiguration();
            }
            ~Process() {
                this->isolate->Dispose();
                V8::Dispose();
                V8::DisposePlatform();
                delete this->create_params.array_buffer_allocator;
                delete log;
            }
            void CreateGlobal() {
                if(this->global.IsEmpty()) {
                    this->global = ObjectTemplate::New(this->isolate);
                }
            }
            Isolate* GetIsolate() {
                return this->isolate;
            }
            Local<Context> GetNewContext(Isolate* isolate) {
                return Context::New(isolate, NULL, this->global);
            }
            bool RunScript(Isolate* isolate, Local<String> source, Local<Value> name, bool print_result=true, bool report_exceptions=true) {
                HandleScope handle_scope(isolate);
                TryCatch try_catch(isolate);
                ScriptOrigin origin(isolate, name);
                Local<Context> context(isolate->GetCurrentContext());
                Local<Script> script;
                if(!Script::Compile(context, source, &origin).ToLocal(&script)) {
                    if(report_exceptions)
                        ReportException(isolate, &try_catch);
                    return false;
                }
                else {
                    Local<Value> result;
                    if(!script->Run(context).ToLocal(&result)) {
                        assert(try_catch.HasCaught());
                        if(report_exceptions)
                            ReportException(isolate, &try_catch);
                        return false;
                    }
                    else {
                        assert(!try_catch.HasCaught());
                        if(print_result && !result->IsUndefined()) {
                            String::Utf8Value str(isolate, result);
                            const char* cstr = ToCString(str);
                            printf("%s\n", cstr);
                        }
                        return true;
                    }
                }
            }
            void ReportException(Isolate* isolate, TryCatch* try_catch) {
                HandleScope handle_scope(isolate);
                String::Utf8Value exception(isolate, try_catch->Exception());
                const char* exception_string = ToCString(exception);
                Local<Message> message = try_catch->Message();
                if(message.IsEmpty()) {
                    fprintf(stderr, "%s\n", exception_string);
                }
                else {
                    // Print (filename):(line number): (message).
                    String::Utf8Value filename(isolate, message->GetScriptOrigin().ResourceName());
                    Local<Context> context(isolate->GetCurrentContext());
                    const char* filename_string = ToCString(filename);
                    int linenum = message->GetLineNumber(context).FromJust();
                    fprintf(stderr, "%s:%i: %s\n", filename_string, linenum, exception_string);
                    // Print line of source code.
                    String::Utf8Value sourceline(isolate, message->GetSourceLine(context).ToLocalChecked());
                    const char* sourceline_string = ToCString(sourceline);
                    fprintf(stderr, "%s\n", sourceline_string);
                    // Print wavy underline (GetUnderline is deprecated).
                    int start = message->GetStartColumn(context).FromJust();
                    for(int i = 0; i < start; i++) {
                        fprintf(stderr, " ");
                    }
                    int end = message->GetEndColumn(context).FromJust();
                    for(int i = start; i < end; i++) {
                        fprintf(stderr, "^");
                    }
                    fprintf(stderr, "\n");
                    Local<Value> stack_trace_string;
                    if(try_catch->StackTrace(context).ToLocal(&stack_trace_string)
                        && stack_trace_string->IsString() && stack_trace_string.As<String>()->Length() > 0) {
                        String::Utf8Value stack_trace(isolate, stack_trace_string);
                        const char* err = ToCString(stack_trace);
                        fprintf(stderr, "%s\n", err);
                    }
                }
            }
    };
};
#endif