#ifndef __SLIM__V8__H
#define __SLIM__V8__H
#include <v8.h>
#include <libplatform/libplatform.h>
namespace slim::veight {
    static bool initilaized = false;
    static v8::Isolate* isolate;
    static v8::Local<v8::ObjectTemplate> global;
    static v8::ExtensionConfiguration* extensions;
    static std::unique_ptr<v8::Platform> platform;
    static v8::Isolate::CreateParams create_params;
    extern void CreateGlobal(void);
    static v8::Local<v8::Script> CompileScript(std::string source, std::string name);
    extern v8::Isolate* GetIsolate();
    extern v8::Local<v8::Context> GetNewContext(void);
    extern void Init(int argc, char* argv[]);
    static void ReportException(v8::TryCatch* try_catch);
    static bool RunScript(v8::Local<v8::Script> script);
    extern void Stop(void);
};
#endif