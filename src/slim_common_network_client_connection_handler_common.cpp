#include <arpa/inet.h>
#include <errno.h>
#include <functional>
#include <future>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <slim/common/network/client/connection/handler/common.h>
#include <slim/common/network/client/connection/information.h>
#include <slim/common/metrics.h>


#include <iostream>
#define BUFFER_SIZE 2048

void slim::common::network::client::connection::handler::common::handle_client_connection(
			slim::common::network::client::connection::Information& client_connection_information,
			std::function<std::string(char*, slim::common::network::client::connection::Information&, slim::common::Metrics&)> client_request_handler,
			slim::common::Metrics& metrics) {
	metrics.counters["threads_started"].inc();
	std::future<std::string> response;
	timeval timeout;
	double  bytes_read = 0;
	double  bytes_written = 0;
	char read_buffer[BUFFER_SIZE];
	std::string response_string;
	fd_set read_set, write_set, except_set;
	if(client_connection_information.read_timeout > -1) {
		timeout.tv_usec = client_connection_information.read_timeout;
	}
	timeout.tv_usec = 1000;

	for(;;) {
		FD_ZERO(&read_set);
		FD_ZERO(&except_set);
		FD_SET(client_connection_information.socket_handle, &read_set);
		int selected = select(client_connection_information.socket_handle + 1, &read_set, NULL, &except_set, NULL);
		if(selected == -1 && errno == EINTR) {
			continue;
		}
		if(selected == -1) {
			break;
		}
		if(FD_ISSET(client_connection_information.socket_handle, &read_set)) {
			memset(read_buffer, '\0', sizeof(read_buffer));
			bytes_read = read(client_connection_information.socket_handle, read_buffer, BUFFER_SIZE);
			if(*read_buffer) {
				response = std::async(client_request_handler, read_buffer, std::ref(client_connection_information), std::ref(metrics));
				response_string = response.get();
			}
			break;
		}
		if(FD_ISSET(client_connection_information.socket_handle, &except_set)) {
			char c;
			bytes_read += recv(client_connection_information.socket_handle, &c, 1, MSG_OOB);
			if(bytes_read < 1) {
				break;
			}
			else {
				memset(read_buffer, '\0', sizeof(read_buffer));
				read_buffer[0] = c;
			}   
		}
	}
	std::cout << "waiting to write\n";
	for(;;) {
		FD_ZERO(&write_set);
		FD_ZERO(&except_set);
		FD_SET(client_connection_information.socket_handle, &write_set);
		int selected = select(client_connection_information.socket_handle + 1, NULL, &write_set, NULL, NULL);
		if(selected == -1 && errno == EINTR) {
			continue;
		}
		if(selected == -1) {
			break;
		}
		if(FD_ISSET(client_connection_information.socket_handle, &write_set)) {
			bytes_written = send(client_connection_information.socket_handle, response_string.c_str(), response_string.length(), 0);
			break;
		}
	}
	shutdown(client_connection_information.socket_handle, SHUT_WR);
	close(client_connection_information.socket_handle);
	metrics.counters["threads_ended"].inc();
}
