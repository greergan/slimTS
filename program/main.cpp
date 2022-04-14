#define VERSION "0.0"
#include <iostream>
#include <slim_v8.hpp>
/* using v8::Context;
using v8::HandleScope;
using v8::Isolate;
using v8::Local;
using v8::String; */
int main(int argc, char *argv[]) {
    SlimV8 slim_v8(argc, argv);

    Isolate* isolate = slim_v8.GetIsolate();
    Isolate::Scope isolate_scope(isolate);
    HandleScope handle_scope(isolate);
    slim_v8.CreateGlobal();
    slim_v8.RegisterFunctions();
    Local<Context> context = slim_v8.GetNewContext(isolate);
    if(context.IsEmpty()) {
        fprintf(stderr, "Error creating context\n");
        return 0;
    }
    else {
        Context::Scope context_scope(context);
        Local<String> source = String::NewFromUtf8Literal(isolate, "print(1+4, 'hello', {message:'world'})");
        Local<String> name = String::NewFromUtf8Literal(isolate, "test");
        bool success = slim_v8.RunScript(isolate, source, name);
    }

    std::cout << "here we go\n";
    return 0;
}