#include <v8.h>
/* namespace slim::fetch {
    struct fetch {
        fetch(const v8::FunctionCallbackInfo<v8::Value>& args) {
            v8::Isolate* isolate = args.GetIsolate();
            auto resolver = v8::Promise::Resolver::New(isolate->GetCurrentContext()).ToLocalChecked();
            args.GetReturnValue().Set(resolver->GetPromise());
            v8::Maybe<bool> result = resolver->Resolve(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "world").ToLocalChecked());
        }
    };
}; */
    struct fetch {
        fetch(const v8::FunctionCallbackInfo<v8::Value>& args) {
            v8::Isolate* isolate = args.GetIsolate();
            auto resolver = v8::Promise::Resolver::New(isolate->GetCurrentContext()).ToLocalChecked();
            args.GetReturnValue().Set(resolver->GetPromise());
            v8::Maybe<bool> result = resolver->Resolve(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "world").ToLocalChecked());
        }
    };