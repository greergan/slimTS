#include <iostream>

#include <v8.h>
#include <slim/builtins.h>
#include <slim/plugin.hpp>
#include <slim/gv8.h>

/* look at source file product/google/v8/src/init/bootstrapper.cc */


//void slim::builtins::initialize(v8::Isolate* isolate) {
void slim::builtins::initialize(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate>& globalObjectTemplate) {
	v8::HandleScope scope(isolate);
	globalObjectTemplate->Set(isolate, "Headers2", v8::FunctionTemplate::New(isolate, new_headers));

/* 	slim::plugin::plugin module(isolate, "module");
	module.expose_plugin();
	slim::plugin::plugin require(isolate, "require");
	require.expose_plugin(); */
	//slim::plugin::plugin headers_builtin(isolate, "Headers", &new_headers);
	//headers_builtin.add_function("append", &append_headers);
	//headers_builtin.expose_plugin();


/* 	slim::plugin::plugin headers(isolate, "Headers");
	headers.add_function("append", append_headers);
	headers.expose_plugin(); */
/* 	slim::plugin::plugin headers(isolate, "Headers", new_headers);
	slim::plugin::plugin append(isolate, "append", append_headers);
	headers.add_plugin("append", &append);
	headers.expose_plugin(); */

/* 	auto object_name = slim::utilities::StringToName(isolate, "Headers");
	auto object_template = v8::ObjectTemplate::New(isolate);
	object_template->SetCallAsFunctionHandler(new_headers);
	auto append = v8::FunctionTemplate::New(isolate, append_headers);
	auto append_name = slim::utilities::StringToName(isolate, "append");
	object_template->Set(append_name, append);
	isolate->GetCurrentContext()->Global()->Set(isolate->GetCurrentContext(), object_name, object_template->NewInstance(isolate->GetCurrentContext()).ToLocalChecked()).ToChecked(); */

/* 	auto headers_name = slim::utilities::StringToName(isolate, "Headers");
	auto append_name = slim::utilities::StringToName(isolate, "append");
	auto headers_object_template = v8::ObjectTemplate::New(isolate);
	headers_object_template->SetCallAsFunctionHandler(new_headers);

	v8::Local<v8::Function> append = v8::FunctionTemplate::New(isolate, append_headers);
	headers_object_template->Set(isolate, "append", append_headers);
	//headers_object_template->Set(isolate, "append", append); */

	/* auto headers_object = headers_object_template->NewInstance(isolate->GetCurrentContext()).ToLocalChecked();
	isolate->GetCurrentContext()->Global()->Set(
		isolate->GetCurrentContext(),
		headers_name,
		headers_object).ToChecked(); */
////////////////////
/* 	auto headers_name = slim::utilities::StringToName(isolate, "Headers");
	v8::Local<v8::FunctionTemplate> headers = v8::FunctionTemplate::New(isolate, new_headers);
	v8::Local<v8::Template> headers_template = headers->PrototypeTemplate();
	v8::Local<v8::FunctionTemplate> append = v8::FunctionTemplate::New(isolate, append_headers);
	headers_template->Set(isolate, "append", append);

	auto headers_object_template = v8::ObjectTemplate::New(isolate);
	headers_object_template->SetCallAsFunctionHandler(new_headers);
	headers_object_template->Set(headers_template); */

/* 
	auto object_name = slim::utilities::StringToName(isolate, "Headers");
	auto new_headers_function_template = v8::FunctionTemplate::New(isolate, new_headers);
	auto headers_object_template = v8::ObjectTemplate::New(isolate, new_headers_function_template);
	/#/object_template->SetCallAsFunctionHandler(new_headers);

	auto append_name = slim::utilities::StringToName(isolate, "append");
	auto headers_append_function_template = v8::FunctionTemplate::New(isolate, append_headers);
	headers_object_template->Set(append_name, headers_append_function_template);

	isolate->GetCurrentContext()->Global()->Set(
		isolate->GetCurrentContext(),
		object_name,
		headers_object_template->NewInstance(isolate->GetCurrentContext()).ToLocalChecked()).ToChecked(); */
}


void slim::builtins::append_headers(const v8::FunctionCallbackInfo<v8::Value>& args) {
	std::cout << "slim::builtins::append_headers\n";
}
void slim::builtins::new_headers(const v8::FunctionCallbackInfo<v8::Value>& args) {
	std::cout << "slim::builtins::new_headers\n";
	auto isolate = args.GetIsolate();
	auto context = isolate->GetCurrentContext();
	//slim::network::http::Headers new_header;
	if(args.Length() == 0) {
		
	}
	auto object = v8::Object::New(isolate);
	auto headers = slim::utilities::StringToString(isolate, "headers");
	auto key = v8::Private::New(isolate, headers);
	auto map = v8::Map::New(isolate);
	auto result = object->SetPrivate(context, key, map);
	args.GetReturnValue().Set(object);
}