#ifndef __SLIM__COMMON__METRICS__COUNTER__H
#define __SLIM__COMMON__METRICS__COUNTER__H
#include <atomic>
#include <string>
namespace slim::common::metrics {
	struct Counter {
		std::string help;
		std::string type;
		std::string format;
		std::atomic<double> count{};
		void inc(void);
		void inc(double value);
	};
}
#endif