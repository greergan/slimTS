#ifndef __SLIM__UV__H
#define __SLIM__UV__H
#include <uv.h>
namespace slim::uv {
    static uv_loop_t* main_loop;
    static uv_signal_t *sigint;
    extern uv_loop_t* get_loop(void);
    extern void handle_libuv_error(const char* message, int error);
    extern void init(void);
    static void on_sigint_received(uv_signal_t *handle, int signum);
    extern void start(void);
    extern void stop(void);
}
#endif