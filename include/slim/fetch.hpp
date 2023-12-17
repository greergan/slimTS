#include <iostream>
#include <regex>
#include <v8.h>
#include <slim/plugin.hpp>
namespace slim::network::fetch {
    void expose_plugin(v8::Isolate* isolate);
    void fetch(const v8::FunctionCallbackInfo<v8::Value>& args);
}
void slim::network::fetch::expose_plugin(v8::Isolate* isolate) {
	slim::plugin::plugin fetch_plugin(isolate, "fetch", &slim::network::fetch::fetch);
    fetch_plugin.expose_plugin();
}
void slim::network::fetch::fetch(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    auto context = isolate->GetCurrentContext();
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);
    v8::Context::Scope context_scope(context);
    const std::regex url_pattern("((http|https)://)[a-zA-Z0-9@:%._\\+~#?&//=]{2,256}\\.[a-z]{2,6}\\b([-a-zA-Z0-9@:%._\\+~#?&//=]*)");
    if(args.Length() == 0) {
        isolate->ThrowException(slim::utilities::StringToString(isolate, "fetch requires at least 1 argument"));
    }
    if(!args[0]->IsObject() && !args[0]->IsString()) {
        isolate->ThrowException(slim::utilities::StringToValue(isolate, "argument 1 of fetch must be a valid url string or a valid Request object"));
    }
    if(args[0]->IsString()) {
        std::string passed_url = slim::utilities::v8ValueToString(isolate, args[0]);
        if(!std::regex_match(passed_url, url_pattern)) {
            isolate->ThrowException(slim::utilities::StringToValue(isolate, "argument 1 of fetch must be a valid url string or a valid Request object"));
        }
    }
    else if(args[0]->IsObject()) {

    }

    auto resolver = v8::Promise::Resolver::New(context).ToLocalChecked();
    auto result = resolver->Resolve(context, slim::utilities::StringToValue(isolate, "hello world"));
    args.GetReturnValue().Set(resolver->GetPromise());
}
