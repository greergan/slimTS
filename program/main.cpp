#define VERSION "0.0"
#include <iostream>
#include <uv.h>
#include <slim_v8.hpp>
#include <slim_server.hpp>
int main(int argc, char *argv[]) {
    static uv_loop_t* main_loop = uv_default_loop();
    slim::log::system::init(main_loop);
    slim::log::system::notice("Slim server starting");
    slim::veight::Process process(argc, argv);
    Isolate* isolate = process.GetIsolate();
    Isolate::Scope isolate_scope(isolate);
    HandleScope handle_scope(isolate);
    process.CreateGlobal();
    process.RegisterFunctions();
    Local<Context> context = process.GetNewContext(isolate);
    if(context.IsEmpty()) {
        slim::log::system::critical("Error creating context");
        exit(1);
    }
    else {
        slim::log::system::notice("Slim server initialized");
        slim::server::Http server(main_loop);
/*         Context::Scope context_scope(context);
        Local<String> source = String::NewFromUtf8Literal(isolate, "print(1+4, 'hello', {message:'world'})");
        Local<String> name = String::NewFromUtf8Literal(isolate, "test");
        bool success = slim_v8.RunScript(isolate, source, name); */
    }
    slim::log::system::notice("Slim server started");
    slim::log::system::handle_libuv_error("slim::main::loop error: ", uv_run(main_loop, UV_RUN_DEFAULT));
    return 0;
}