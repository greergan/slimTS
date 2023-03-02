#ifndef __SLIM__UV__HPP
#define __SLIM__UV__HPP
#include <uv.h>
#include <log.hpp>
namespace slim::uv {
    static uv_loop_t* main_loop;
    uv_signal_t *sigint;
    uv_loop_t* GetLoop(void);
    void init(void);
    void on_sigint_received(uv_signal_t *handle, int signum);
    void start(void);
    void stop(void);
    uv_loop_t* GetLoop() {
        return main_loop;
    }
    void init() {
        sigint = new uv_signal_t;
        uv_signal_init(uv_default_loop(), sigint);
        uv_signal_start(sigint, on_sigint_received, SIGINT);
        main_loop = uv_default_loop();
    }
    void on_sigint_received(uv_signal_t *handle, int signum) {
        if(uv_loop_close(handle->loop) == UV_EBUSY) {
            uv_walk(handle->loop, [](uv_handle_t* handle, void* arg) { uv_close(handle, NULL); }, NULL);
        }
    }
    void start() {
        slim::log::handle_libuv_error("slim::uv::loop error: ", uv_run(main_loop, UV_RUN_DEFAULT));
    }
    void stop() {
        uv_loop_close(main_loop);
        if(sigint != NULL) {
            delete sigint;
        }            
    }
}
#endif