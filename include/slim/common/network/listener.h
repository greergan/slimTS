#ifndef __SLIM__COMMON__NETWORK__LISTENER__H
#define __SLIM__COMMON__NETWORK__LISTENER__H
#include <functional>
#include <slim/common/metrics.h>
#include <slim/common/network/listener/information.h>
namespace slim::common::network::listener {
	void standard(Information& listener_information,
		std::function<std::string(char* request_pointer, slim::common::network::client::connection::Information& client_connection_information, slim::common::Metrics&)> client_request_handler,
		slim::common::Metrics& metrics
	);
};
#endif