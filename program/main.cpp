#define VERSION "0.0"
#include <iostream>
#include <uv.h>
#include <slim_v8.hpp>

uv_loop_t *loop;
static void on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
 /*    if(nread < 0) {
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
    request_complete(request); */
    free(buf->base);
}
static void on_close(uv_handle_t* peer) {
  free(peer);
}
static void on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
    buf->base = (char *)malloc(suggested_size);
    buf->len = suggested_size;
}
static void on_connection(uv_stream_t* server, int status) {
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
    r = uv_tcp_init(loop, (uv_tcp_t*) stream);
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
}

int main(int argc, char *argv[]) {
    SlimV8 slim_v8(argc, argv);
    Isolate* isolate = slim_v8.GetIsolate();
    Isolate::Scope isolate_scope(isolate);
    HandleScope handle_scope(isolate);
    slim_v8.CreateGlobal();
    slim_v8.RegisterFunctions();
    Local<Context> context = slim_v8.GetNewContext(isolate);
    if(context.IsEmpty()) {
        fprintf(stderr, "Error creating context\n");
        return 0;
    }
    else {
        loop = uv_default_loop();
        uv_tcp_t server;
        sockaddr_in addr;
        uv_tcp_init(loop, &server);
        uv_ip4_addr("0.0.0.0", 8080, &addr);
        uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0);
        int r = uv_listen((uv_stream_t*) &server, 100, on_connection);
        if(r) {
            fprintf(stderr, "Listen error %s\n", uv_strerror(r));
            return 1;
        }
        std::cout << "Starting loop\n";
        return uv_run(loop, UV_RUN_DEFAULT);

/*         Context::Scope context_scope(context);
        Local<String> source = String::NewFromUtf8Literal(isolate, "print(1+4, 'hello', {message:'world'})");
        Local<String> name = String::NewFromUtf8Literal(isolate, "test");
        bool success = slim_v8.RunScript(isolate, source, name); */
    }
    return 0;
}