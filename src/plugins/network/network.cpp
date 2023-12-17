#include <iostream>

#include <v8.h>
#include <asio.hpp>
#include <slim/utilities.h>
#include <slim/plugin.hpp>
#include <slim/v8.hpp>
namespace slim::network {
	namespace server {
		namespace ip {
			void listen(const v8::FunctionCallbackInfo<v8::Value>& args);
			asio::awaitable<void> listener();
		}
	}
}
void slim::network::server::ip::listen(const v8::FunctionCallbackInfo<v8::Value>& args) {
	//asio::buffer
	//asio::ip::tcp
	// host => 192.168.122.33
	// service => http
/* 	std::string_view host("192.168.122.33");
	std::string_view service("8500");
	asio::io_context server_context;
	auto listen_endpoint = *asio::ip::tcp::resolver(server_context).resolve(
		host, service, asio::ip::tcp::resolver::passive
	);
	asio::ip::tcp::acceptor acceptor(server_context, listen_endpoint);
	asio::ip::tcp::socket server_socket(server_context); */


	//acceptor.async_accept(slim::network::server::ip::listener);
/* 	asio::ip::tcp::socket client_socket(server_context);
	client_socket.connect(acceptor.local_endpoint()); */
	//server_context.run();
	try {
		asio::io_context io_context(1);
		asio::signal_set signals(io_context, SIGINT, SIGTERM);
		signals.async_wait([&](auto, auto){ io_context.stop(); });
		asio::co_spawn(io_context, listener(), asio::detached);
		io_context.run();
	}
	catch (std::exception& e) {
		std::printf("Exception: %s\n", e.what());
	}
}
asio::awaitable<void> slim::network::server::ip::listener() {
	auto executor = co_await asio::this_coro::executor;
	asio::ip::tcp::acceptor acceptor(executor, {asio::ip::tcp::v4(), 8500});
	for(;;) {
		asio::ip::tcp::socket socket = co_await acceptor.async_accept(asio::use_awaitable);
		std::string data;
		std::size_t n = co_await asio::async_read_until(socket, asio::dynamic_buffer(data, 1024), "\n", asio::use_awaitable);
		data.erase(0,n);

		std::cout << n << " " << data << "\n";
		//co_spawn(executor, echo(std::move(socket)), asio::detached);
	}
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