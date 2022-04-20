#ifndef __SLIM__UTILITIES__HPP
#define __SLIM__UTILITIES__HPP
#include <string>
#include <v8.h>
namespace slim::utilies {
    std::string ValueToString(v8::Isolate* isolate, v8::Local<v8::Value> value) {
        v8::String::Utf8Value utf8_value(isolate, value);
        return std::string(*utf8_value);
    }
}
#endif