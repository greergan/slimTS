#include <sstream>
#include <v8.h>
#include <slim/common/log.h>
#include <slim/exception_handler.h>
#include <slim/utilities.h>
namespace slim::exception_handler {
	using namespace slim::common;
	using namespace slim::utilities;
}
/* 	void PrintStackTrace(Isolate* isolate, Local<Value> error) {
		if(error->IsObject()) {
			Local<Object> error_obj = error->ToObject(isolate->GetCurrentContext()).ToLocalChecked();
			Local<String> message_key = String::NewFromUtf8Literal(isolate, "message");
			Local<Value> message_value;
			Local<String> stack_key = String::NewFromUtf8Literal(isolate, "stack");
			Local<Value> stack_value;
			if(error_obj->Get(isolate->GetCurrentContext(), StringToV8String(isolate, "stack")).ToLocal(&stack_value) && stack_value->IsString()) {
				cout << "\n" << v8ValueToString(isolate, stack_value) << "\n";
			}
			return;
		}
    	cout << "\n" << v8ValueToString(isolate, error) << "\n";
	}
	void OnPromiseRejected(PromiseRejectMessage message) {
		Isolate* isolate = message.GetPromise()->GetIsolate();
		Local<Value> value = message.GetValue();
		PrintStackTrace(isolate, value);
	} */

void slim::exception_handler::v8_try_catch_handler(v8::TryCatch* try_catch) {
	log::trace(log::Message("slim::exception_handler::try_catch_handler()","begins",__FILE__, __LINE__));
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
	log::trace(log::Message("slim::exception_handler::try_catch_handler()","ends",__FILE__, __LINE__));
	throw(exception_string.str());
}