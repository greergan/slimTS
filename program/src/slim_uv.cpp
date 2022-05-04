#include <slim_uv.h>
#include <log.h>
#include <stdlib.h>
uv_loop_t* slim::uv::GetLoop() {
    return main_loop;
}
void slim::uv::HandleLIBUVError(const char* message, int error) {
        if(error) {
            slim::log::critical(message, uv_strerror(error));
            exit(error);
        }
    }
void slim::uv::Init() {
    sigint = new uv_signal_t;
    uv_signal_init(uv_default_loop(), sigint);
    uv_signal_start(sigint, OnSigintReceived, SIGINT);
    main_loop = uv_default_loop();
}
void slim::uv::OnSigintReceived(uv_signal_t *handle, int signum) {
    if(uv_loop_close(handle->loop) == UV_EBUSY) {
        uv_walk(handle->loop, [](uv_handle_t* handle, void* arg) { uv_close(handle, NULL); }, NULL);
    }
}
void slim::uv::Start() {
    HandleLIBUVError("slim::uv::loop error: ", uv_run(main_loop, UV_RUN_DEFAULT));
}
void slim::uv::Stop() {
    uv_loop_close(main_loop);
    if(sigint != NULL) {
        delete sigint;
    }
}