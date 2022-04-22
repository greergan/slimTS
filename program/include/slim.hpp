#ifndef __SLIM__HPP
#define __SLIM__HPP
#include <v8.h>
#include <v8pp/module.hpp>
#include <uv.hpp>
#include <log.hpp>
#include <veight.hpp>
#include <modules.hpp>
#include <http_server.hpp>
namespace slim {
    static uv_loop_t* slim_loop;
    void init() {
        slim_loop = slim::uv::init();
        slim::log::init(slim_loop);
        slim::log::info("slim starting");
        slim::http::init(slim_loop);
    }
    void start() {
        slim::uv::start();
    }
    void stop() {
        slim::http::stop();
        slim::log::info("slim stopping");
        slim::uv::stop();
    }
    void expose(v8::Isolate* isolate) {
        slim::modules::expose(isolate);
    }
}
#endif