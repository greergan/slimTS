#ifndef __SLIM__MODULES__H
#define __SLIM__MODULES__H
#include <v8.h>
#include <uv.h>
namespace slim::modules {
    extern void Expose(v8::Isolate* isolate);
}
#endif