#ifndef __SLIM__HPP
#define __SLIM__HPP
#include <v8.h>
#include <v8pp/module.hpp>
#include <uv.hpp>
#include <log.hpp>
#include <veight.hpp>
#include <modules.hpp>
#include <console.hpp>
#include <http_server.hpp>
namespace slim {
    void expose(void);
    void stop(void);
    void start(int argc, char* argv[]) {
        if(argc == 2) {
            std::cerr << argv[1] << "\n";
/*             slim::uv::init();
            slim::log::init(slim::uv::GetLoop());
            slim::http::init(slim::uv::GetLoop());
            slim::veight::init(argc, argv); */
            //run(const std::string source);
        }
    }
    void stop() {
        slim::veight::stop();
        slim::uv::stop();
    }
    void expose() {
        slim::modules::expose(slim::veight::GetIsolate());
    }
    void run(const std::string source) {
        if(source.length() < 2) {
            return;
        }
        auto isolate = slim::veight::GetIsolate();
        v8::Isolate::Scope isolate_scope(isolate);
        v8::HandleScope handle_scope(isolate);
        slim::veight::CreateGlobal();
        v8::Local<v8::Context> context = slim::veight::GetNewContext();
        if(context.IsEmpty()) {
            throw("Error creating context");
        }
        v8::Context::Scope context_scope(context);
        expose();
        v8::TryCatch try_catch(isolate);
        v8::Local<v8::Script> script = slim::veight::CompileScript(source, "console_testing");
        if(try_catch.HasCaught()) {
            slim::veight::ReportException(&try_catch);
        }
        if(!script.IsEmpty()) {
            bool result = slim::veight::RunScript(script);
            if(try_catch.HasCaught()) {
                slim::veight::ReportException(&try_catch);
            }
            return;
        }
        else {
            return;
        }
        slim::uv::start();
    }
}
#endif