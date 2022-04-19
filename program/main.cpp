#define VERSION "0.0"
#include <iostream>
#include <uv.h>
#include <veight.hpp>
void on_uv_walk(uv_handle_t* handle, void* arg) { uv_close(handle, NULL); }
void on_sigint_received(uv_signal_t *handle, int signum) {
    slim::log::system::notice("Slim server shutting down");
    int result = uv_loop_close(handle->loop);
    if(result == UV_EBUSY) {
        uv_walk(handle->loop, on_uv_walk, NULL);
    }
}
int main(int argc, char *argv[]) {
    uv_signal_t *sigint = new uv_signal_t;
    uv_signal_init(uv_default_loop(), sigint);
    uv_signal_start(sigint, on_sigint_received, SIGINT);
    static uv_loop_t* main_loop = uv_default_loop();
    slim::http::init(main_loop);
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
        v8::Isolate::Scope isolate_scope(isolate);
        HandleScope handle_scope(isolate);
        Context::Scope context_scope(context);
        v8::Local<v8::Value> log_instance = slim::log::system::expose(isolate);
        v8::Maybe result = context->Global()->Set(isolate->GetCurrentContext(), v8pp::to_v8(isolate, "log"), log_instance);
        if(!result.IsNothing()) {}

        Local<String> source = String::NewFromUtf8Literal(isolate, "http();log.notice('hello world');");
        Local<String> name = String::NewFromUtf8Literal(isolate, "test");
        bool success = process.RunScript(isolate, source, name);
    }
    slim::log::system::handle_libuv_error("slim::main::loop error: ", uv_run(main_loop, UV_RUN_DEFAULT));
    uv_loop_close(uv_default_loop());
    slim::http::stop();
    delete sigint;
    return 0;
}