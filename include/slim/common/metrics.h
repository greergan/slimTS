#ifndef __SLIM__COMMON__METRICS__H
#define __SLIM__COMMON__METRICS__H
#include <string>
#include <unordered_map>
#include <slim/common/metrics/counter.h>
namespace slim::common {
	struct Metrics {
		std::string pid_string;
		std::unordered_map<std::string, slim::common::metrics::Counter> counters;
		void add_counter(std::string key, std::string help, std::string type, std::string format);
		Metrics(void);
	};
};
#endif
