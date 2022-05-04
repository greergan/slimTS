#include <slim.h>
#include <fstream>
#include <v8.h>
#include <slim_v8.h>
#include <slim_uv.h>
void slim::expose() {
    //slim::modules::expose(slim::veight::GetIsolate());
}
void slim::run(const std::string file_name, const std::string file_contents) {
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
void slim::start(int argc, char* argv[]) {
    if(argc == 2) {
        std::string file_contents;
        std::string file_name{argv[1]};
        std::ifstream file(file_name);
        getline(file, file_contents, '\0');
        file.close();
        if(file_contents.length() > 2) {
            slim::uv::init();
            //slim::log::init(slim::uv::get_loop());
            //slim::http::init(slim::uv::get_loop());
            slim::veight::init(argc, argv);
            run(file_name, file_contents);
            stop();
        }
    }
}
void slim::stop() {
    slim::veight::stop();
    slim::uv::stop();
}