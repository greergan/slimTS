#ifndef __SLIM__HPP
#define __SLIM__HPP
#include <v8.h>
#include <v8pp/module.hpp>
#include <slim_uv.h>
#include <log.hpp>
#include <veight.hpp>
#include <modules.hpp>
#include <console.hpp>
#include <http_server.hpp>
#include <fstream>
namespace slim {
    void expose(void);
    void run(const std::string file_name, const std::string file_contents);
    void stop(void);
    void start(int argc, char* argv[]) {
        if(argc == 2) {
            std::string file_contents;
            std::string file_name{argv[1]};
            std::ifstream file(file_name);
            getline(file, file_contents, '\0');
            file.close();
            if(file_contents.length() > 2) {
                slim::uv::init();
                slim::log::init(slim::uv::get_loop());
                slim::http::init(slim::uv::get_loop());
                slim::veight::init(argc, argv);
                run(file_name, file_contents);
                stop();
            }
        }
    }
    void stop() {
        slim::veight::stop();
        slim::uv::stop();
    }
    void expose() {
        slim::modules::expose(slim::veight::GetIsolate());
    }
    void run(const std::string file_name, const std::string file_contents) {
        auto isolate = slim::veight::GetIsolate();
        v8::Isolate::Scope isolate_scope(isolate);
        v8::HandleScope handle_scope(isolate);
        slim::veight::CreateGlobal();
        auto context = slim::veight::GetNewContext();
        if(context.IsEmpty()) {
            throw("Error creating context");
        }
        v8::Context::Scope context_scope(context);
        slim::expose();
        v8::TryCatch try_catch(isolate);
        auto script = slim::veight::CompileScript(file_contents, file_name);
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