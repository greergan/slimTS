#ifndef __SLIM__SERVER__H
#define __SLIM__SERVER__H
#include <uv.h>
#include <v8.h>
#include <string>
namespace slim::server {
    struct Server {
        uv_loop_t* loop;
        uv_tcp_t http_server;
        sockaddr_in addr;
    };
};
namespace slim::server::http {
    static slim::server::Server server;
    extern void Init(uv_loop_t *loop);
    static void on_allocate(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
    static void on_close(uv_handle_t* peer);
    static void on_connection(uv_stream_t* server, const int status);
    static void on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
    extern bool Start(const int port, const std::string host);
    extern void Start(const v8::FunctionCallbackInfo<v8::Value> &args);
    extern void Stop(void);
};
#endif