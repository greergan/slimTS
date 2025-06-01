#include <unistd.h>
#include <future>
#include <sstream>
#include <string>
#include <slim/common/metrics.h>
#include <slim/common/metrics/counter.h>
slim::common::Metrics::Metrics(void) {
	auto pid_response = std::async(getpid);
	std::stringstream pid_format_stream;
	pid_format_stream << "{pid=\"" << pid_response.get() << "\"}";
	pid_string = pid_format_stream.str();
}
void slim::common::Metrics::add_counter(std::string key, std::string help, std::string type, std::string format) {
	slim::common::metrics::Counter counter{.help=help, .type=type, .format=format, .count=0};
	counters[key].help = help;
	counters[key].type = type;
	counters[key].count = 0;
	int pid_token_position = format.find("%pid");
	if(pid_token_position > 0) {
		format.replace(pid_token_position, 4, pid_string);
	}
	counters[key].format = format;
};
void slim::common::metrics::Counter::inc(void) {
	count = count + 1;
}
void slim::common::metrics::Counter::inc(double value) {
	count = count + value;
}
