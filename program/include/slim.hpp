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
    static uv_loop_t* slim_loop;
    void init(int argc, char* argv[]) {
        slim_loop = slim::uv::init();
        slim::log::init(slim_loop);
        slim::log::info("slim starting");
        slim::http::init(slim_loop);
        slim::veight::init(argc, argv);
    }
    void start() {
        v8::Isolate::Scope isolate_scope(slim::veight::isolate);
        v8::HandleScope handle_scope(slim::veight::isolate);
        slim::veight::CreateGlobal();
        v8::Local<v8::Context> context = slim::veight::GetNewContext();
        if(context.IsEmpty()) {
            throw("Error creating context");
        }
        v8::Context::Scope context_scope(context);
        expose();
        std::string source = R"(
            console.clear();
            const http_stream = slim.http({"port": 8080, "host": "0.0.0.0"});
            console.configure({"log": {"custom_text_color": 208, "custom_background_color" : 237, "italic": true }});
            console.log("console.log");
            console.configure({"dir": {"custom_text_color": 22, "custom_background_color" : 237, "bold": true }});
            console.dir("console.dir");
            console.error("console.error");
            console.debug("console.debug");
            console.info("console.info");
            console.todo("console.todo");
            console.configure({"trace": {"propogate": true }});
            console.trace("console.trace");
            console.configure({"warn":  {"remainder": {"inherit": true}}});
            console.warn("console.warn");
            console.configure({"todo":  {"remainder": {"inherit": true}}});
            console.todo("finish console.configure.propogate");
            console.todo("add slim.configuration.console.log.text_color = value");
        )";
        v8::TryCatch try_catch(slim::veight::isolate);
        v8::Local<v8::Script> script = slim::veight::CompileScript(source, "console_testing");
        if(try_catch.HasCaught()) {
            slim::veight::ReportException(&try_catch);
        }
        if(!script.IsEmpty()) {
            bool result = slim::veight::RunScript(script);
            if(try_catch.HasCaught()) {
                slim::veight::ReportException(&try_catch);
            }
            stop();
        }
        slim::uv::start();
    }
    void stop() {
        slim::http::stop();
        slim::log::info("slim stopping");
        slim::veight::stop();
        slim::uv::stop();
    }
    void expose() {
        slim::modules::expose(slim::veight::isolate);
    }
}
#endif