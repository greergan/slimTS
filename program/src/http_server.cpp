#include <string_view>
#include <server.h>
#include <utilities.h>
#include <uv.h>
#include <log.h>
void slim::server::http::Init(uv_loop_t *loop) {
    server.loop = loop;
    slim::utilities::HandleLibUVError("TCP initilization failed: ", uv_tcp_init(server.loop, &server.http_server));
}
void slim::server::http::on_allocate(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
    buf->base = (char *)malloc(suggested_size);
    buf->len = suggested_size;
}
void slim::server::http::on_close(uv_handle_t* peer) {
    free(peer);
}
void slim::server::http::on_connection(uv_stream_t* server, const int status) {
    slim::log::info("slim::server::http::connection");
    if(status) {
        slim::log::warn("Connect error: ", uv_err_name(status), ": ", uv_strerror(status));
        return;
    }
}
void slim::server::http::on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
    if(nread <= 0) {
        if(buf) {
            free(buf->base);
            return;
        }
    }
    free(buf->base);
}
bool slim::server::http::Start(const int port, const std::string host) {
    slim::utilities::HandleLibUVError("Address error: ", uv_ip4_addr(host.c_str(), port, &server.addr));
    slim::utilities::HandleLibUVError("Bind error: ", uv_tcp_bind(&server.http_server, (const struct sockaddr*)&server.addr, 0));
    slim::utilities::HandleLibUVError("Listen error: ", uv_listen((uv_stream_t*) &server.http_server, 512, on_connection));
    slim::log::notice("slim::server::http listening on ", host, ":", port);
    return true;
}
void slim::server::http::Start(const v8::FunctionCallbackInfo<v8::Value> &args) {
    if(args.Length() == 0) {
        slim::log::debug("throw error 1 in slim::http::start");
    }
    else {
        v8::Isolate* isolate = args.GetIsolate();
        v8::HandleScope scope(isolate);
        v8::Local<v8::Object> start_arguments = slim::utilities::GetObject(isolate, args[0]);
        if(!start_arguments.IsEmpty()) {
            std::string host = slim::utilities::StringValue(isolate, "host", start_arguments);
            int port = slim::utilities::IntValue(isolate, "port", start_arguments);
            if(port > 0 && host != "undefined") {
                if(Start(port, host)) {
                    args.GetReturnValue().SetEmptyString();
                }
            }
            else {
                slim::log::debug("throw error 2 in slim::http::start");
            }
        }
    }
}