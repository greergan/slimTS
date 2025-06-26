#ifndef __SLIM__COMMON__NETWORK__LISTENER__H
#define __SLIM__COMMON__NETWORK__LISTENER__H
#include <functional>
#include <memory>
#include <slim/common/metrics.h>
#include <slim/common/network/client/connection/information.h>
#include <slim/common/network/listener/information.h>
namespace slim::common::network::listener {
	using client_request_handler_function 
		= std::function<std::string(char* request_pointer, 
			const slim::common::network::client::connection::Information& client_connection_information, slim::common::Metrics&)>;
	void standard(std::shared_ptr<network::listener::Information> listener_information,
					client_request_handler_function client_request_handler, slim::common::Metrics& metrics);
};
#endif