#ifndef __SLIM__NETWORK__FETCH__H
#define __SLIM__NETWORK__FETCH__H
namespace slim::network::fetch {
    void expose_plugin(v8::Isolate* isolate);
    void fetch(const v8::FunctionCallbackInfo<v8::Value>& args);
}
#endif