#include <iostream>
#include <v8.h>
#include <slim/gv8.h>
#include <slim/plugin.hpp>
#include <slim/utilities.h>
namespace slim::network {
	namespace server {
		namespace ip {
			void listen(const v8::FunctionCallbackInfo<v8::Value>& args);
		}
	}
}
void slim::network::server::ip::listen(const v8::FunctionCallbackInfo<v8::Value>& args) {
}
extern "C" void expose_plugin(v8::Isolate* isolate) {
	slim::plugin::plugin net_plugin(isolate, "net");
	slim::plugin::plugin client_plugin(isolate, "client");
	slim::plugin::plugin server_plugin(isolate, "server");
	server_plugin.add_function("listen", &slim::network::server::ip::listen);
	net_plugin.add_plugin("client", &server_plugin);
	net_plugin.add_plugin("server", &server_plugin);
	net_plugin.expose_plugin();
}