#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <v8.h>
#include <libplatform/libplatform.h>
#include <slim/utilities.h>
using namespace slim::utilities;
v8::MaybeLocal<v8::Module> module_resolve_callback(v8::Local<v8::Context> context, v8::Local<v8::String> specifier, 
                                                  v8::Local<v8::FixedArray> import_assertions, v8::Local<v8::Module> referrer) {
    v8::Isolate* isolate = context->GetIsolate();
    v8::MaybeLocal<v8::Module> result;
    std::string specifier_str = v8StringToString(isolate, specifier);
    std::ifstream module_file(specifier_str);
    if (module_file.is_open()) {
        std::stringstream buffer;
        buffer << module_file.rdbuf();
        v8::ScriptOrigin origin(specifier, 0, 0, false, -1, StringToV8Value(isolate, ""), false, false, true);
	    v8::ScriptCompiler::Source v8_module_source(StringToV8String(isolate, buffer.str()), origin);
	    v8::ScriptCompiler::CompileOptions module_compile_options(v8::ScriptCompiler::kNoCompileOptions);
	    v8::ScriptCompiler::NoCacheReason module_no_cache_reason(v8::ScriptCompiler::kNoCacheNoReason);
        v8::TryCatch try_catch(isolate);
	    auto module = v8::ScriptCompiler::CompileModule(isolate, &v8_module_source, module_compile_options, module_no_cache_reason);
        if(try_catch.HasCaught()) {
            std::cout << "caught error\n";
        }
        if(module.IsEmpty()) {
          auto error_string = v8ValueToString(isolate, try_catch.Exception());
          std::cerr << "Error CompileModule module => " << error_string << std::endl;
        }
        module.ToLocalChecked()->InstantiateModule(context, module_resolve_callback);
        if(try_catch.HasCaught()) {
            std::cout << "caught error\n";
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

      v8::Local<v8::String> main_code_v8 = StringToV8String(isolate, main_buffer.str().c_str());
      auto specifier_string = v8::String::NewFromUtf8(isolate, "main.mjs").ToLocalChecked();
      v8::ScriptOrigin origin(StringToV8Value(isolate, "main.mjs"), 0, 0, false, -1, StringToV8Value(isolate, ""), false, false, true);
	    v8::ScriptCompiler::Source v8_module_source(main_code_v8, origin);
	    v8::ScriptCompiler::CompileOptions module_compile_options(v8::ScriptCompiler::kNoCompileOptions);
	    v8::ScriptCompiler::NoCacheReason module_no_cache_reason(v8::ScriptCompiler::kNoCacheNoReason);
    v8::TryCatch try_catch(isolate);
	      auto main_module = v8::ScriptCompiler::CompileModule(isolate, &v8_module_source, module_compile_options, module_no_cache_reason);
        if(try_catch.HasCaught()) {
            std::cout << "caught error\n";
        }
      if(main_module.IsEmpty()) {
          auto error_string = v8ValueToString(isolate, try_catch.Exception());
          std::cerr << "Error compiling module => " << error_string << std::endl;
          return 1;
      }
	    auto result = main_module.ToLocalChecked()->InstantiateModule(context, module_resolve_callback);
                if(try_catch.HasCaught()) {
            std::cout << "caught error\n";
        }
	    if (result.IsNothing()) {
          auto error_string = v8ValueToString(isolate, try_catch.Exception());
          std::cerr << "Error instantiating module => " << error_string << std::endl;
          return 1;
      }
      auto evaluate_result = main_module.ToLocalChecked()->Evaluate(context);
      if(evaluate_result.IsEmpty()) {
          auto error_string = v8ValueToString(isolate, try_catch.Exception());
          std::cerr << "Error evaluating module => " << error_string << std::endl;
          return 1;
      }
      std::cout << v8ValueToString(isolate, evaluate_result.ToLocalChecked()) << "\n";
      std::cout << "evaluation results => " << evaluate_result.IsEmpty()<< "\n";
      //if (evaluate_result.ToLocalChecked()->StaState() != v8::Promise::kPending)
      std::cout << v8ValueToString(isolate, evaluate_result.ToLocalChecked() )<< "\n";
  }
  // Dispose V8 objects
  isolate->Dispose();
  v8::V8::Dispose();
  v8::V8::DisposePlatform();
  delete create_params.array_buffer_allocator;
  return 0;
}