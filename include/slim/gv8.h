#ifndef __SLIM__V8__HPP
#define __SLIM__V8__HPP
#include <sstream>
#include <v8.h>
#include <libplatform/libplatform.h>
namespace slim::gv8 {
    struct Gv8Config {
        bool initialized = false;
        v8::Isolate* isolate;
        v8::Local<v8::ObjectTemplate> globalObjectTemplate;
        v8::ExtensionConfiguration* extensions;
        std::unique_ptr<v8::Platform> platform;
        v8::Isolate::CreateParams create_params;
    };
    v8::Local<v8::Module> CompileAndInstantiateModule(std::string source, std::string name);
    void FetchCompileAndRunJSFunction(v8::Local<v8::Context> context, std::string file_name_string);
    v8::Local<v8::Script> CompileScript(std::string source, std::string name);
    void CreateGlobalTemplate(void);
    v8::Isolate* GetIsolate();
    v8::Local<v8::ObjectTemplate>& GetGlobalObjectTemplate(void);
    v8::Local<v8::Context> GetNewContext(void);
    v8::MaybeLocal<v8::Module> ModuleCallbackResolver(v8::Local<v8::Context> context, v8::Local<v8::String> specifier, 
                                                    v8::Local<v8::FixedArray> import_assertions, v8::Local<v8::Module> referrer);
    void ReportException(v8::TryCatch* try_catch);
    bool RunScript(v8::Local<v8::Script> script);
    void initialize(int argc, char* argv[]);
    void stop(void);
};
#endif