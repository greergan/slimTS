#include <regex>
#include <string>
#include <v8.h>
#include <slim/utilities.h>
int slim::utilities::ArrayCount(v8::Local<v8::Value> value) {
	if(value->IsArray()) {
		return v8::Handle<v8::Array>::Cast(value)->Length();
	}
	return 0;
}
template <typename Thing>
v8::Local<v8::Object> slim::utilities::GetObject(v8::Isolate* isolate, Thing thingy) {
	return (thingy->IsObject()) ? thingy->ToObject(isolate->GetCurrentContext()).ToLocalChecked() : v8::Object::New(isolate);
}
v8::Local<v8::Object> slim::utilities::GetObject(v8::Isolate* isolate, v8::Local<v8::Value> object) {
	return (object->IsObject()) ? object->ToObject(isolate->GetCurrentContext()).ToLocalChecked() : v8::Object::New(isolate);
}
v8::Local<v8::Object> slim::utilities::GetObject(v8::Isolate* isolate, std::string string, v8::Local<v8::Object> object) {
	return GetObject(isolate, GetValue(isolate, string, object));
}
v8::Local<v8::Value> slim::utilities::GetValue(v8::Isolate* isolate, std::string string, v8::Local<v8::Object> object) {
	return object->Get(isolate->GetCurrentContext(), StringToName(isolate, string)).ToLocalChecked();
}
int slim::utilities::IntValue(v8::Isolate* isolate, v8::Local<v8::Value> value) {
	if(!value->IsInt32()) {
		isolate->ThrowException(slim::utilities::StringToString(isolate, "integer value expected"));
	}  
	return value->Int32Value(isolate->GetCurrentContext()).FromJust();
}
int slim::utilities::IntValuePositive(v8::Isolate* isolate, std::string string, v8::Local<v8::Object> object) {
	v8::Local<v8::Value> value = GetValue(isolate, string, object);
	if(value->IsInt32()) {
		return value->Int32Value(isolate->GetCurrentContext()).FromJust();
	}
	return -1;
}
double slim::utilities::NumberValue(v8::Isolate* isolate, v8::Local<v8::Value> value) {
	return value->NumberValue(isolate->GetCurrentContext()).FromJust();
}
int slim::utilities::PropertyCount(v8::Isolate *isolate, v8::Local<v8::Object> object) {
	return object->GetPropertyNames(isolate->GetCurrentContext()).ToLocalChecked()->Length();
}
std::string slim::utilities::ScriptFileName(v8::Local<v8::Message> message) {
	return StringValue(message->GetIsolate(), message->GetScriptOrigin().ResourceName());
}
std::string slim::utilities::ScriptLine(v8::Local<v8::Message> message) {
		return StringValue(message->GetIsolate(), message->GetSourceLine(message->GetIsolate()->GetCurrentContext()).ToLocalChecked());
}
int slim::utilities::ScriptLineNumber(v8::Local<v8::Message> message) {
	return message->GetLineNumber(message->GetIsolate()->GetCurrentContext()).FromJust();
}
std::string slim::utilities::ScriptStackTrace(v8::TryCatch* try_catch) {
	auto isolate = try_catch->Message()->GetIsolate();
	return StringValue(isolate, try_catch->StackTrace(isolate->GetCurrentContext()).ToLocalChecked());
}
bool slim::utilities::BoolValue(v8::Isolate* isolate, v8::Local<v8::Value> value) {
	if(!value->IsBoolean()) {
		isolate->ThrowException(slim::utilities::StringToString(isolate, "boolean value expected"));
	}  
	return value->BooleanValue(isolate);
}
std::string slim::utilities::SlimColorValue(v8::Isolate* isolate, v8::Local<v8::Value> value, std::vector<std::string>& colors) {
	std::string return_value;
	if(value->IsString()) {
		auto color_string = StringValue(isolate, value);
//             for (auto i = std::sregex_iterator(printf_string.begin(), printf_string.end(), format); i != std::sregex_iterator(); ++i) {
//                std::cout << i->str() << '\n';
//            }
		if(std::regex_match(color_string, std::regex(".+;.+;.+"))) {
			std::smatch matches;
			std::regex rgb_code("([0-9]{1,3});([0-9]{1,3});([0-9]{1,3})");
			if(regex_search(color_string, matches, rgb_code)) {
				for(int i = 1; i < 4; i++) {
					int code = stoi(matches[i]);
					if(code < 0 || code > 255) {
						isolate->ThrowException(slim::utilities::StringToString(isolate, color_string + " codes must be 0-255"));
					}
				}
				return_value = color_string;
			}
			else {
				isolate->ThrowException(slim::utilities::StringToString(isolate, color_string + " unsupported rgb format"));
			}
		}
		else {
			auto find_result = std::find(std::begin(colors), std::end(colors), color_string);
			if(find_result != std::end(colors)) {
				return_value = color_string;
			}
			else {
				std::stringstream message;
				message << color_string << " not in supported ASCII colors\n";
				message << "try console.colors()";
				isolate->ThrowException(slim::utilities::StringToString(isolate, message.str()));
			}
		}
	}
	else if(value->IsInt32()) {
		auto int_color = slim::utilities::IntValue(isolate, value);
		if(int_color > 255 || int_color < 0) {
			isolate->ThrowException(slim::utilities::StringToString(isolate, "extended ASCII codes range 0-255"));
		}
		else {
			return_value = std::to_string(int_color);
		}
	}
	return return_value;
}
std::string slim::utilities::StringFunction(v8::Isolate* isolate, v8::Local<v8::Value> function) {
	return StringValue(isolate, function->ToString(isolate->GetCurrentContext()).ToLocalChecked());
}
v8::Local<v8::Name> slim::utilities::StringToName(v8::Isolate* isolate, std::string string) {
	return v8::String::NewFromUtf8(isolate, string.c_str()).ToLocalChecked();
}
v8::Local<v8::String> slim::utilities::StringToString(v8::Isolate* isolate, std::string string) {
	return v8::String::NewFromUtf8(isolate, string.c_str()).ToLocalChecked();
}
v8::Local<v8::Value> slim::utilities::StringToValue(v8::Isolate* isolate, std::string string) {
	return v8::String::NewFromUtf8(isolate, string.c_str()).ToLocalChecked();
}
template <typename Thing>
std::string slim::utilities::StringValue(v8::Isolate* isolate, Thing thingy) {
	v8::String::Utf8Value utf8_value(isolate, thingy);
	return std::string(*utf8_value);
}
std::string slim::utilities::StringValue(v8::Isolate* isolate, v8::Local<v8::Value> value) {
	v8::String::Utf8Value utf8_value(isolate, value);
	return std::string(*utf8_value);	
}
std::string slim::utilities::StringValue(v8::Isolate* isolate, v8::Local<v8::String> string) {
	v8::String::Utf8Value utf8_value(isolate, string);
	return std::string(*utf8_value);	
}
std::string slim::utilities::StringValue(v8::Isolate* isolate, std::string string, v8::Local<v8::String> value) {
	v8::String::Utf8Value utf8_value(isolate, value);
	return std::string(*utf8_value);
}
std::string slim::utilities::StringValue(v8::Isolate* isolate, std::string string, v8::Local<v8::Object> object) {
	return StringValue(isolate, GetValue(isolate, string, object));
}
std::string slim::utilities::V8JsonValueToString(v8::Isolate* isolate, v8::Local<v8::Value> value) {
    return(std::string("slim::utilities::V8JsonValueToString not implemented"));
}
std::string slim::utilities::v8StringToString(v8::Isolate* isolate, v8::Local<v8::String> string) {
	v8::String::Utf8Value string_value(isolate, string);
	return std::string(*string_value);
}
std::string slim::utilities::v8ValueToString(v8::Isolate* isolate, v8::Local<v8::Value> value) {
	v8::String::Utf8Value string_value(isolate, value);
	return std::string(*string_value);
}
