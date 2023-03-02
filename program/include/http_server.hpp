#ifndef __SLIM__HTTP__SERVER__HPP
#define __SLIM__HTTP__SERVER__HPP
#include <uv.h>
#include <stdlib.h>
#include <string>
#include <string_view>
#include <log.hpp>
#include <utilities.hpp>
namespace slim::http {
    struct Server {
        uv_loop_t* loop;
        uv_tcp_t http_server;
        sockaddr_in addr;
    } server;
    void init(uv_loop_t *loop);
    static void allocate(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
    static void on_close(uv_handle_t* peer);
    static void on_connection(uv_stream_t* server, const int status);
    bool start(const int port, const std::string_view host);
    void start(const v8::FunctionCallbackInfo<v8::Value> &args);
    void stop(void);
    static void allocate(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
        buf->base = (char *)malloc(suggested_size);
        buf->len = suggested_size;
    }
    void init(uv_loop_t *loop) {
        server.loop = loop;
        slim::log::handle_libuv_error("TCP initilization failed: ", uv_tcp_init(server.loop, &server.http_server));
    }
    static void on_close(uv_handle_t* peer) {
        free(peer);
    }
    static void on_connection(uv_stream_t* server, const int status) {
        slim::log::info("slim::http::server::connection");
        if(status) {
            slim::log::warn("Connect error: ", uv_err_name(status), ": ", uv_strerror(status));
            return;
        }
    }
    static void on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
        if(nread <= 0) {
            if(buf) {
                free(buf->base);
                return;
            }
        }
        free(buf->base);
    }
    bool start(const int port, const std::string host) {
        slim::log::handle_libuv_error("Address error: ", uv_ip4_addr(host.c_str(), port, &server.addr));
        slim::log::handle_libuv_error("Bind error: ", uv_tcp_bind(&server.http_server, (const struct sockaddr*)&server.addr, 0));
        slim::log::handle_libuv_error("Listen error: ", uv_listen((uv_stream_t*) &server.http_server, 512, on_connection));
        slim::log::notice("slim::http::server listening on ", host, ":", port);
        return true;
    }
    void start(const v8::FunctionCallbackInfo<v8::Value> &args) {
        if(args.Length() == 0) {
            slim::log::debug("throw error 1 in slim::http::start");
        }
        else {
            v8::Isolate* isolate = args.GetIsolate();
            v8::HandleScope scope(isolate);
            v8::Local<v8::Object> start_arguments = slim::utilities::GetObject(isolate, args[0]);
            if(!start_arguments.IsEmpty()) {
                std::string host = slim::utilities::StringValue(isolate, "host", start_arguments);
                int port = slim::utilities::IntValuePositive(isolate, "port", start_arguments);
                if(port > 0 && host != "undefined") {
                    if(start(port, host)) {
                        args.GetReturnValue().SetEmptyString();
                    }
                }
                else {
                    slim::log::debug("throw error 2 in slim::http::start");
                }
            }
        }
    }
};
#endif
/*
static void on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
    if(nread < 0) {
        if(buf->base)
            free(buf->base);
        return;
    }
    if(nread == 0) {
        free(buf->base);
        return;
    }
    http_request* request = calloc(1, sizeof(http_request));
    if(request == NULL) {
        free(buf->base);
        fprintf(stderr, "Allocate error: %s\n", strerror(errno));
        uv_close((uv_handle_t*) stream, on_close);
        return;
    }
    stream->data = request;
    request->handle = (uv_handle_t*) stream;
    request->num_headers = sizeof(request->headers) / sizeof(request->headers[0]);
    int nparsed = phr_parse_request(
        buf->base,
        buf->len,
        &request->method,
        &request->method_len,
        &request->path,
        &request->path_len,
        &request->minor_version,
        request->headers,
        &request->num_headers,
        0);
    if(nparsed < 0) {
        free(request);
        free(buf->base);
        fprintf(stderr, "Invalid request\n");
        uv_close((uv_handle_t*) stream, on_close);
        return;
    }
  
    // TODO: handle reading whole payload
    request->payload = buf->base + nparsed;
    request->payload_len = buf->len - nparsed;
    request_complete(request);
    free(buf->base);
}*/
/* static void on_connection(uv_stream_t* server, int status) {
    uv_stream_t* stream;
    int r;
    if(status != 0) {
        fprintf(stderr, "Connect error: %s: %s\n", uv_err_name(status), uv_strerror(status));
        return;
    }
    stream = (uv_stream_t *)malloc(sizeof(uv_tcp_t));
    if(stream == NULL) {
        fprintf(stderr, "Allocate error: %s\n", strerror(errno));
        return;
    }
    r = uv_tcp_init(server_loop, (uv_tcp_t*) stream);
    if(r) {
        fprintf(stderr, "Socket creation error: %s: %s\n", uv_err_name(r), uv_strerror(r));
    return;
    }
    r = uv_tcp_simultaneous_accepts((uv_tcp_t*) stream, 1);
    if(r) {
        fprintf(stderr, "Flag error: %s: %s\n", uv_err_name(r), uv_strerror(r));
        return;
    }
    r = uv_accept(server, stream);
    if(r) {
        fprintf(stderr, "Accept error: %s: %s\n", uv_err_name(r), uv_strerror(r));
        return;
    }
    r = uv_tcp_nodelay((uv_tcp_t*) stream, 1);
    if(r) {
        fprintf(stderr, "Flag error: %s: %s\n", uv_err_name(r), uv_strerror(r));
        return;
    }
    r = uv_read_start(stream, on_alloc, on_read);
    if(r) {
        fprintf(stderr, "Read error: %s: %s\n", uv_err_name(r), uv_strerror(r));
        uv_close((uv_handle_t*) stream, on_close);
    }
} */