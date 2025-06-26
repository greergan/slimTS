#ifndef __SLIM__COMMON__NETWORK__CLIENT__CONNECTION__HANDLER__COMMON__H
#define __SLIM__COMMON__NETWORK__CLIENT__CONNECTION__HANDLER__COMMON__H
#include <functional>
#include <string>
#include <slim/common/network/client/connection/information.h>
#include <slim/common/metrics.h>
namespace slim::common::network::client::connection::handler::common {
	void handle_client_connection(
		slim::common::network::client::connection::Information& client_connection_information,
		std::function<std::string(char*, slim::common::network::client::connection::Information&, slim::common::Metrics&)> client_request_handler,
		slim::common::Metrics& metrics
	);
}
#endif