#include <slim_uv.h>
//#include <log.hpp>
#include <stdlib.h>
uv_loop_t* slim::uv::get_loop() {
    return main_loop;
}
void slim::uv::handle_libuv_error(const char* message, int error) {
        if(error) {
            //slim::log::critical(message, uv_strerror(error));
            exit(error);
        }
    }
void slim::uv::init() {
    sigint = new uv_signal_t;
    uv_signal_init(uv_default_loop(), sigint);
    uv_signal_start(sigint, on_sigint_received, SIGINT);
    main_loop = uv_default_loop();
}
void slim::uv::on_sigint_received(uv_signal_t *handle, int signum) {
    if(uv_loop_close(handle->loop) == UV_EBUSY) {
        uv_walk(handle->loop, [](uv_handle_t* handle, void* arg) { uv_close(handle, NULL); }, NULL);
    }
}
void slim::uv::start() {
    handle_libuv_error("slim::uv::loop error: ", uv_run(main_loop, UV_RUN_DEFAULT));
}
void slim::uv::stop() {
    uv_loop_close(main_loop);
    if(sigint != NULL) {
        delete sigint;
    }
}