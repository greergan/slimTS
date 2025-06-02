#ifndef __SLIM__COMMON__NETWORK__CLIENT__REQUEST__HANDLER__STANDARD__H
#define __SLIM__COMMON__NETWORK__CLIENT__REQUEST__HANDLER__STANDARD__H
#include <slim/common/network/client/connection/information.h>
#include <slim/common/metrics.h>
namespace slim::common::network::client::request::handler::standard {
	std::string handle_request(char* data, slim::common::network::client::connection::Information& client_connection_information, slim::common::Metrics& metrics);
}
#endif