#ifndef __SLIM__COMMON__NETWORK__CLIENT__CONNECTION__INFORMATION__H
#define __SLIM__COMMON__NETWORK__CLIENT__CONNECTION__INFORMATION__H
#include <netinet/in.h>
namespace slim::common::network::client::connection {
	struct Information {
		int read_timeout;
		int socket_handle;
		struct sockaddr_in client_address;
	};
}
#endif