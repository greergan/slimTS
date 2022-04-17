#ifndef __SLIM__HTTP__SERVER__HPP
#define __SLIM__HTTP__SERVER__HPP
#include <logger.hpp>
#include <uv.h>
#include <stdlib.h>
#include <string>
namespace slim::http {
    uv_loop_t* server_loop;
    void init(uv_loop_t *loop) { server_loop = loop; }
    struct Server {
        private:
            uv_tcp_t server;
            int error;
        public:
        Server(const int port=8080, const char* ip_address="0.0.0.0") {
            sockaddr_in addr;
            slim::log::system::handle_libuv_error("TCP initilization failed: ", uv_tcp_init(server_loop, &server));
            slim::log::system::handle_libuv_error("Address error: ", uv_ip4_addr(ip_address, port, &addr));
            slim::log::system::handle_libuv_error("Bind error: ", uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0));
            slim::log::system::handle_libuv_error("Listen error: ", uv_listen((uv_stream_t*) &server, 512, on_connection));
            slim::log::system::notice("Slim HTTP server listening on ", ip_address, " port ", port);
        }
        ~Server() {
            std::cout << "destructing\n";
        }
        static void allocate(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
            buf->base = (char *)malloc(suggested_size);
            buf->len = suggested_size;
        }
        static void on_close(uv_handle_t* peer) { free(peer); }
        static void on_connection(uv_stream_t* server, const int status) {
            slim::log::system::info("New HTTP connection");
            if(status) {
                slim::log::system::warn("Connect error: ", uv_err_name(status), ": ", uv_strerror(status));
                return;
            }
        }
        static void on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
            if(nread <= 0) { if(buf) free(buf->base); return; }
            free(buf->base);
        }
    };
    Server* http_server;
    void start(const int port=8080, const char* ip_address="0.0.0.0") {
        http_server = new Server(port, ip_address);
    }
    void stop() { if(http_server != NULL) delete http_server; }
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