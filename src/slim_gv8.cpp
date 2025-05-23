#include <array>
#include <vector>
#include <iostream>
#include <set>
#include <v8.h>
#include <slim/gv8.h>
#include <libplatform/libplatform.h>
#include <slim/utilities.h>
#include <slim/common/fetch.h>
#include <slim/common/fetch_and_apply_macros.h>
#include <slim/common/log.h>
#include <slim/plugin/loader.h>
namespace slim::gv8 {
	using namespace slim;
	using namespace slim::common;
	Gv8Config slim_v8;
}
void slim::gv8::CreateGlobalTemplate() {
	slim_v8.globalObjectTemplate = v8::ObjectTemplate::New(slim_v8.isolate);
}
void slim::gv8::FetchCompileAndRunJSFunction(v8::Local<v8::Context> context, std::string file_name_string) {
	log::trace(log::Message("slim::gv8::CompileAndRunJSFunction()","begins",__FILE__, __LINE__));
	auto* isolate = context->GetIsolate();
	v8::EscapableHandleScope scope(isolate);
	v8::Local<v8::String> source(utilities::StringToV8String(isolate, fetch::fetch(file_name_string.c_str()).str()));
/* 	
	if (!LoadBuiltinSource(isolate, id).ToLocal(&source)) {
	  return {};
	} */
  
	v8::ScriptOrigin origin(utilities::StringToV8String(isolate, file_name_string), 0, 0, true);

/* 	BuiltinCodeCacheData cached_data{};
	{
	  // Note: The lock here should not extend into the
	  // `CompileFunction()` call below, because this function may recurse if
	  // there is a syntax error during bootstrap (because the fatal exception
	  // handler is invoked, which may load built-in modules).
	  RwLock::ScopedLock lock(code_cache_->mutex);
	  auto cache_it = code_cache_->map.find(id);
	  if (cache_it != code_cache_->map.end()) {
		// Transfer ownership to ScriptCompiler::Source later.
		cached_data = cache_it->second;
	  }
	} */
  
	//const bool has_cache = cached_data.data != nullptr;
	const bool has_cache = false;
	const bool should_eager_compile_ = false;
	v8::ScriptCompiler::CompileOptions options =
		has_cache ? v8::ScriptCompiler::kConsumeCodeCache
				  : v8::ScriptCompiler::kNoCompileOptions;
/* 	if (should_eager_compile_) {
	  options = ScriptCompiler::kEagerCompile;
	} else if (!to_eager_compile_.empty()) {
	  if (to_eager_compile_.find(id) != to_eager_compile_.end()) {
		options = ScriptCompiler::kEagerCompile;
	  }
	} */
	//ScriptCompiler::Source script_source(source, origin, has_cache ? cached_data.AsCachedData().release() : nullptr);
	v8::ScriptCompiler::Source script_source(source, origin, nullptr);
  
/* 	per_process::Debug(
		DebugCategory::CODE_CACHE,
		"Compiling %s %s code cache %s\n",
		id,
		has_cache ? "with" : "without",
		options == ScriptCompiler::kEagerCompile ? "eagerly" : "lazily"); */
  
	auto maybe_fun = v8::ScriptCompiler::CompileFunction(context, &script_source); // parameters->size(), parameters->data(), 0, nullptr, options);
	//ScriptCompiler::CompileFunction(context, &script_source, parameters->size(), parameters->data(), 0, nullptr, options);
	
	// This could fail when there are early errors in the built-in modules,
	// e.g. the syntax errors
	v8::Local<v8::Function> fun;
	if (!maybe_fun.ToLocal(&fun)) {
	  // In the case of early errors, v8 is already capable of
	  // decorating the stack for us - note that we use CompileFunction
	  // so there is no need to worry about wrappers.
	  //return MaybeLocal<Function>();
	  return;
	}

	// XXX(joyeecheung): this bookkeeping is not exactly accurate because
	// it only starts after the Environment is created, so the per_context.js
	// will never be in any of these two sets, but the two sets are only for
	// testing anyway.
  
/* 	Result result = (has_cache && !script_source.GetCachedData()->rejected)
						? Result::kWithCache
						: Result::kWithoutCache;
	if (optional_realm != nullptr) {
	  DCHECK_EQ(this, optional_realm->env()->builtin_loader());
	  RecordResult(id, result, optional_realm);
	} */
  
/* 	if (has_cache) {
	  per_process::Debug(DebugCategory::CODE_CACHE,
						 "Code cache of %s (%s) %s\n",
						 id,
						 script_source.GetCachedData()->buffer_policy ==
								 ScriptCompiler::CachedData::BufferNotOwned
							 ? "BufferNotOwned"
							 : "BufferOwned",
						 script_source.GetCachedData()->rejected ? "is rejected"
																 : "is accepted");
	} */
/*   
	if (result == Result::kWithoutCache && optional_realm != nullptr &&
		!optional_realm->env()->isolate_data()->is_building_snapshot()) {
	  // We failed to accept this cache, maybe because it was rejected, maybe
	  // because it wasn't present. Either way, we'll attempt to replace this
	  // code cache info with a new one.
	  // This is only done when the isolate is not being serialized because
	  // V8 does not support serializing code cache with an unfinalized read-only
	  // space (which is what isolates pending to be serialized have).
	  SaveCodeCache(id, fun);
	} */

	//return scope.Escape(fun);
	scope.Escape(fun);
	int argc;
	v8::Local<v8::Value> arguments[] = {};
	v8::Local<v8::Value> undefined = Undefined(context->GetIsolate());
	auto results = fun->Call(context, utilities::StringToV8Value(isolate, file_name_string), 0, arguments);
	log::trace(log::Message("slim::gv8::CompileAndRunJSFunction()","ends",__FILE__, __LINE__));
}
v8::Local<v8::Script> slim::gv8::CompileScript(std::string source, std::string name) {
	log::trace(log::Message("slim::gv8::CompileScript()","begins",__FILE__, __LINE__));
	v8::TryCatch try_catch(slim_v8.isolate);
	v8::MaybeLocal<v8::Script> script = v8::Script::Compile(slim_v8.isolate->GetCurrentContext(), utilities::StringToString(slim_v8.isolate, source));
	if(try_catch.HasCaught()) {
		log::error(log::Message("slim::gv8::CompileScript()","try_catch.HasCaught()",__FILE__, __LINE__));
		ReportException(&try_catch);
	}
	log::trace(log::Message("slim::gv8::CompileScript()","ends",__FILE__, __LINE__));
	return script.ToLocalChecked();
}
v8::Local<v8::ObjectTemplate>& slim::gv8::GetGlobalObjectTemplate() {
	return slim_v8.globalObjectTemplate;
}
v8::Isolate* slim::gv8::GetIsolate() {
	return slim_v8.isolate;
}
v8::Local<v8::Context> slim::gv8::GetNewContext() {
	return v8::Context::New(slim_v8.isolate, NULL, slim_v8.globalObjectTemplate);
}
void slim::gv8::initialize(int argc, char* argv[]) {
	v8::V8::InitializeICUDefaultLocation(argv[0]);
	slim_v8.platform = v8::platform::NewDefaultPlatform();
	v8::V8::InitializePlatform(slim_v8.platform.get());
	v8::V8::Initialize();
	v8::V8::SetFlagsFromCommandLine(&argc, argv, true);
	slim_v8.create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
	slim_v8.isolate = v8::Isolate::New(slim_v8.create_params);
	slim_v8.isolate->Enter();
	slim_v8.initialized = true;
}
void slim::gv8::ReportException(v8::TryCatch* try_catch) {
	log::trace(log::Message("slim::gv8::ReportException","begins",__FILE__, __LINE__));
	auto* isolate = try_catch->Message()->GetIsolate();
	auto context = isolate->GetCurrentContext();
	auto script_origin = try_catch->Message()->GetScriptOrigin();
	auto message = try_catch->Message();
	auto stack_trace = try_catch->StackTrace(context).ToLocalChecked();

	log::debug(log::Message("script_origin.ScriptId()", std::to_string(script_origin.ScriptId()).c_str(),__FILE__, __LINE__));
	log::debug(log::Message("script_origin.ColumnOffset()", std::to_string(script_origin.ColumnOffset()).c_str(),__FILE__, __LINE__));
	log::debug(log::Message("script_origin.LineOffset()", std::to_string(script_origin.LineOffset()).c_str(),__FILE__, __LINE__));
	log::debug(log::Message("script_origin.ResourceName()", utilities::v8ValueToString(isolate, script_origin.ResourceName()).c_str(),__FILE__, __LINE__));
	log::debug(log::Message("message->Get()", utilities::v8StringToString(isolate, message->Get()).c_str(),__FILE__, __LINE__));
	log::debug(log::Message("message->GetScriptResourceName()", utilities::v8ValueToString(isolate, message->GetScriptResourceName()).c_str(),__FILE__, __LINE__));
	log::debug(log::Message("message->GetSourceLine()", utilities::v8StringToString(isolate, message->GetSourceLine(context).ToLocalChecked()).c_str(),__FILE__, __LINE__));
	log::debug(log::Message("message->ErrorLevel()", std::to_string(message->ErrorLevel()).c_str(),__FILE__, __LINE__));
	log::debug(log::Message("message->GetStartColumn()", std::to_string(message->GetStartColumn()).c_str(),__FILE__, __LINE__));
	log::debug(log::Message("message->GetEndColumn()", std::to_string(message->GetEndColumn()).c_str(),__FILE__, __LINE__));
	log::debug(log::Message("message->GetStartPosition()", std::to_string(message->GetStartPosition()).c_str(),__FILE__, __LINE__));
	log::debug(log::Message("message->GetStartPosition()", std::to_string(message->GetEndPosition()).c_str(),__FILE__, __LINE__));
	

	std::stringstream exception_string;
	if(!script_origin.ResourceName()->IsUndefined()) {
		exception_string << "\n" << utilities::v8ValueToString(isolate, script_origin.ResourceName());
	}
	if(try_catch->HasCaught()) {
		exception_string << "\n" << utilities::v8ValueToString(isolate, try_catch->Exception()) << "\n";
	}
	exception_string << utilities::v8ValueToString(isolate, message->GetSourceLine(context).ToLocalChecked()) << "\n";
	for(int column = 0; column < message->GetStartColumn(); column++) {
		exception_string << " ";
	}
	exception_string << "^" << std::endl;
	if(!stack_trace.IsEmpty()) {
		exception_string << "\n" << utilities::v8ValueToString(isolate, stack_trace);
	}
	log::trace(log::Message("slim::gv8::ReportException","ends => throw(exception_string)",__FILE__, __LINE__));
	throw(exception_string.str());
}
bool slim::gv8::RunScript(v8::Local<v8::Script> script) {
	if(!script.IsEmpty()) {
		v8::TryCatch try_catch(slim_v8.isolate);
		v8::MaybeLocal<v8::Value> result = script->Run(slim_v8.isolate->GetCurrentContext());
		if(try_catch.HasCaught()) {
			ReportException(&try_catch);
		}
	}
	return true;
}
void slim::gv8::stop() {
	if(slim_v8.initialized) {
		if(slim_v8.isolate != NULL) {
			slim_v8.isolate->Exit();
			slim_v8.isolate->Dispose();
		}
		v8::V8::Dispose();
		v8::V8::DisposePlatform();
		delete slim_v8.create_params.array_buffer_allocator;
		slim_v8.initialized = false;
	}
}