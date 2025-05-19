#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <v8.h>
#include <libplatform/libplatform.h>
#include <slim/utilities.h>
using namespace slim::utilities;
using namespace v8;
void PrintStackTrace(Isolate* isolate, Local<Value> error) {
    if(error->IsObject()) {
        Local<Object> error_obj = error->ToObject(isolate->GetCurrentContext()).ToLocalChecked();
        Local<String> message_key = String::NewFromUtf8Literal(isolate, "message");
        Local<Value> message_value;
        Local<String> stack_key = String::NewFromUtf8Literal(isolate, "stack");
        Local<Value> stack_value;
/*     if(error_obj->Get(isolate->GetCurrentContext(), message_key).ToLocal(&message_value) && message_value->IsString()) {
        String::Utf8Value message_str(isolate, message_value);
        std::cout << "message:\n" << *message_str << std::endl;
    } */
        if(error_obj->Get(isolate->GetCurrentContext(), StringToV8String(isolate, "stack")).ToLocal(&stack_value) && stack_value->IsString()) {
            std::cout << "\n" << v8ValueToString(isolate, stack_value) << "\n";
        }
        return;
    }
    std::cout << "\n" << v8ValueToString(isolate, error) << "\n";
}

void OnPromiseRejected(PromiseRejectMessage message) {
    Isolate* isolate = message.GetPromise()->GetIsolate();
    Local<Value> value = message.GetValue();
    PrintStackTrace(isolate, value);
}
v8::MaybeLocal<v8::Module> module_resolve_callback(v8::Local<v8::Context> context, v8::Local<v8::String> specifier, 
                                                  v8::Local<v8::FixedArray> import_assertions, v8::Local<v8::Module> referrer) {
    v8::Isolate* isolate = context->GetIsolate();
    v8::MaybeLocal<v8::Module> result;
    v8::String::Utf8Value str(isolate, specifier);
    std::string specifier_str(*str);
    std::ifstream module_file(specifier_str);
    if (module_file.is_open()) {
        std::stringstream buffer;
        buffer << module_file.rdbuf();
        v8::ScriptOrigin origin(specifier, 0, 0, false, -1, v8::Local<v8::Value>(), false, false, true);
	    v8::ScriptCompiler::Source v8_module_source(v8::String::NewFromUtf8(isolate, buffer.str().c_str()).ToLocalChecked(), origin);
	    v8::ScriptCompiler::CompileOptions module_compile_options(v8::ScriptCompiler::kEagerCompile);
	    v8::ScriptCompiler::NoCacheReason module_no_cache_reason(v8::ScriptCompiler::kNoCacheNoReason);
        v8::TryCatch try_catch(isolate);
	    auto module = v8::ScriptCompiler::CompileModule(isolate, &v8_module_source, module_compile_options, module_no_cache_reason);
        if(try_catch.HasCaught()) {
            v8::String::Utf8Value str(isolate, try_catch.Exception());
            std::string error_string(*str);
            std::cerr << "Error CompileModule module => " << error_string << std::endl;
        }
        if(module.IsEmpty()) {
            std::cerr << "Error CompileModule module it is empty=> \n";
        }
        module.ToLocalChecked()->InstantiateModule(context, module_resolve_callback);
        if(try_catch.HasCaught()) {
            v8::String::Utf8Value str(isolate, try_catch.Exception());
            std::string error_string(*str);
            std::cerr << "Error InstantiateModule module => " << error_string << std::endl;
        }
        result = module;
    }
    else {
        std::cerr << "Error: Could not open module file: " << specifier_str << std::endl;
    }
    return result;
}

int main(int argc, char* argv[]) {
    // Initialize V8
    v8::V8::InitializeICUDefaultLocation(argv[0]);
    v8::V8::InitializeExternalStartupData("");
    auto platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(platform.get());
    v8::V8::Initialize();
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    v8::Isolate* isolate = v8::Isolate::New(create_params);
    {
        v8::Isolate::Scope isolate_scope(isolate);
        isolate->SetPromiseRejectCallback(OnPromiseRejected);
        v8::HandleScope handle_scope(isolate);
        v8::Local<v8::Context> context = v8::Context::New(isolate);
        v8::Context::Scope context_scope(context);
        std::ifstream main_file("main.mjs");
        if (!main_file.is_open()) {
            std::cerr << "Error: Could not open main.mjs" << std::endl;
            return 1;
        }
        std::stringstream main_buffer;
        main_buffer << main_file.rdbuf();
        v8::Local<v8::String> main_code_v8 = v8::String::NewFromUtf8(isolate, main_buffer.str().c_str()).ToLocalChecked();
        v8::ScriptOrigin origin(v8::String::NewFromUtf8(isolate, "main.mjs").ToLocalChecked(), 0, 0, false, -1, v8::Local<v8::Value>(), false, false, true);
	    v8::ScriptCompiler::Source v8_module_source(main_code_v8, origin);
	    v8::ScriptCompiler::CompileOptions module_compile_options(v8::ScriptCompiler::kEagerCompile);
	    v8::ScriptCompiler::NoCacheReason module_no_cache_reason(v8::ScriptCompiler::kNoCacheNoReason);
        v8::TryCatch try_catch(isolate);
	    auto main_module = v8::ScriptCompiler::CompileModule(isolate, &v8_module_source, module_compile_options, module_no_cache_reason);
	    auto result = main_module.ToLocalChecked()->InstantiateModule(context, module_resolve_callback);
        auto evaluate_result = main_module.ToLocalChecked()->Evaluate(context);
        std::cout << "at the end\n";
    }
  // Dispose V8 objects
  isolate->Dispose();
  v8::V8::Dispose();
  v8::V8::DisposePlatform();
  delete create_params.array_buffer_allocator;
  return 0;
}