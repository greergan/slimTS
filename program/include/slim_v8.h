#ifndef __SLIM__V8__H
#define __SLIM__V8__H
#include <v8.h>
#include <libplatform/libplatform.h>
namespace slim::veight {
    extern void CreateGlobal(void);
    extern v8::Local<v8::Script> CompileScript(std::string source, std::string name);
    extern v8::Isolate* GetIsolate();
    extern v8::Local<v8::Context> GetNewContext(void);
    extern void Init(int argc, char* argv[]);
    extern void ReportException(v8::TryCatch* try_catch);
    extern bool RunScript(v8::Local<v8::Script> script);
    extern void Stop(void);
};
#endif