#define VERSION "0.0"
#include <slim.hpp>
int main(int argc, char *argv[]) {
    slim::init();
    slim::veight::Process process(argc, argv);
    Isolate* isolate = process.GetIsolate();
    Isolate::Scope isolate_scope(isolate);
    HandleScope handle_scope(isolate);
    process.CreateGlobal();
    Local<Context> context = process.GetNewContext(isolate);
    if(context.IsEmpty()) {
        slim::log::critical("Error creating context");
        exit(1);
    }
    else {
        v8::Isolate::Scope isolate_scope(isolate);
        HandleScope handle_scope(isolate);
        Context::Scope context_scope(context);
        slim::expose(isolate);
        Local<String> source = String::NewFromUtf8Literal(isolate, 
        R"(
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
/*             for await (const request of http_stream) {
                handle(request);
            } */
        )");
        Local<String> name = String::NewFromUtf8Literal(isolate, "command_line");
        bool success = process.RunScript(isolate, source, name);
    }
    slim::start();
    slim::stop();
    return 0;
}