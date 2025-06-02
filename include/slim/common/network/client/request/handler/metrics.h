#ifndef __SLIM__COMMON__NETWORK__CLIENT__REQUEST__HANDLER__METRICS__H
#define __SLIM__COMMON__NETWORK__CLIENT__REQUEST__HANDLER__METRICS__H
#include <slim/common/network/client/connection/information.h>
#include <slim/common/http/headers.h>
#include <slim/common/metrics.h>
namespace slim::common::network::client::request::handler::metrics {
	static std::string get_metrics(slim::common::Metrics& metrics);
	std::string handle_request(
		char* data, slim::common::network::client::connection::Information& client_connection_information, slim::common::Metrics& metrics);
}
#endif