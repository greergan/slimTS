#ifndef __SLIM__MODULES__ACCESSORS__HPP
#define __SLIM__MODULES__ACCESSORS__HPP
#include <v8.h>
#include <console.hpp>
#include <utilities.hpp>
namespace slim::modules::accessors {
    void GetWarnTextColor(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
        info.GetReturnValue().Set(
            slim::utilities::StringToValue(info.GetIsolate(), slim::console::configuration::warn.text_color)
        );
    }
    void SetWarnTextColor(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
        slim::console::configuration::warn.text_color = slim::utilities::StringValue(info.GetIsolate(), value);
    }
};
#endif