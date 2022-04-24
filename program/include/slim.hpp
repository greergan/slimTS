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
    void init(int argc, char* argv[]);
    void start(void);
    void stop(void);
    void init(int argc, char* argv[]) {
        slim::uv::init();
        slim::log::init(slim::uv::GetLoop());
        slim::http::init(slim::uv::GetLoop());
        slim::veight::init(argc, argv);
    }
    void stop() {
        slim::veight::stop();
        slim::uv::stop();
    }
    void expose() {
        slim::modules::expose(slim::veight::GetIsolate());
    }
    void RunTest() {
        v8::Isolate::Scope isolate_scope(slim::veight::GetIsolate());
        v8::HandleScope handle_scope(slim::veight::GetIsolate());
        slim::veight::CreateGlobal();
        v8::Local<v8::Context> context = slim::veight::GetNewContext();
        if(context.IsEmpty()) {
            throw("Error creating context");
        }
        v8::Context::Scope context_scope(context);
        expose();
        std::string source = R"(//console.clear();
            console.configure({"todo":  {"remainder": {"inherit": true}}});
            console.todo("finish console.configure.propogate");
            const result = slim.http({"port": 8080, "host": "0.0.0.0"});
            console.log(result);
            slim.log.warn();
            //slim.log();
            //console.todo("add slim.configuration.console.log.text_color = value");
        )";
        v8::TryCatch try_catch(slim::veight::GetIsolate());
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