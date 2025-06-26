#ifndef __SLIM__COMMON__NETWORK__LISTENER__INFORMATION__H
#define __SLIM__COMMON__NETWORK__LISTENER__INFORMATION__H
#include <functional>
#include <slim/common/network/address/set.h>
namespace slim::common::network::listener {
	struct Information {
		slim::common::network::address::AddressSet address_set;
		int back_pressure;
		int read_time_out;
	};
}
#endif