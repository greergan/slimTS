#include <v8.h>
#include <console.h>
#include <module.h>
namespace slim::console::module {
    static slim::module::module AddConfiguration(v8::Isolate* isolate);
    static slim::module::module AddProperties(v8::Isolate* isolate, const std::string level, auto configuration);
    extern void ExposeModule(v8::Isolate* isolate);
};