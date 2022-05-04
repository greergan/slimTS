#ifndef __SLIM__UV__H
#define __SLIM__UV__H
#include <uv.h>
namespace slim::uv {
    static uv_loop_t* main_loop;
    static uv_signal_t *sigint;
    extern uv_loop_t* GetLoop(void);
    extern void HandleLIBUVError(const char* message, int error);
    extern void Init(void);
    static void OnSigintReceived(uv_signal_t *handle, int signum);
    extern void Start(void);
    extern void Stop(void);
}
#endif