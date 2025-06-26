#ifndef __SLIM__COMMON__NETWORK__REQUESTER__URL__H
#define __SLIM__COMMON__NETWORK__REQUESTER__URL__H
#include <slim/common/network/address/set.h>
#include <slim/common/network/protocol.h>
namespace slim::common::network::requester {
	struct Request : slim::common::network::address::AddressSet {
		slim::common::network::protocol protocol;
	};
	struct Url : Request {
		std::string path;
	};
}
#endif