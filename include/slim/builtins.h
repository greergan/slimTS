#ifndef __SLIM__BUILTINS__H
#define __SLIM__BUILTINS__H
#include <v8.h>
namespace slim::builtins {
	void initialize(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate>& globalObjectTemplate);
	void append_headers(const v8::FunctionCallbackInfo<v8::Value>& args);
	void new_headers(const v8::FunctionCallbackInfo<v8::Value>& args);
	void require(const v8::FunctionCallbackInfo<v8::Value>& args);
}
#endif