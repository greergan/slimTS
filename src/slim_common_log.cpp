#include <iostream>
#include <mutex>
#include <slim/common/log.h>
namespace slim::common::log {
	std::mutex cerr_mutex;
	std::mutex cout_mutex;
}
static std::map<std::string, std::string> colors {
	{"cyan", "\033[0;36m"},
	{"blue", "\033[0;34m"},
	{"green", "\033[0;32m"},
	{"purple", "\033[0;35m"},
 	{"red", "\033[0;31m"},
	{"white", "\033[0;37m"},
	{"yellow", "\033[0;33m"}
};
static std::string colorize(const char* input, const char* color) {
	return std::string(colors[const_cast<char*>(color)]) + std::string(input) + "\033[0m";
}
static std::string blue(const char* input) {
	return colorize(input, "blue");
}
static std::string cyan(const char* input) {
	return colorize(input, "cyan");
}
static std::string green(const char* input) {
	return colorize(input, "green");
}
static std::string purple(const char* input) {
	return colorize(input, "purple");
}
static std::string red(const char* input) {
	return colorize(input, "red");
}
static std::string yellow(const char* input) {
	return colorize(input, "yellow");
}
static std::string white(const char* input) {
	return colorize(input, "white");
}
void slim::common::log::all(bool value) {
	allow_debug = value;
	allow_error = value;
	allow_info = value;
	allow_trace = value;
	allow_typescript_warning = value;
}
void slim::common::log::debug(bool value) {
	allow_debug = value;
}
void slim::common::log::debug(const Message message) {
	if(allow_debug)
		print(yellow("DEBUG => "), message);
}
void slim::common::log::error(bool value) {
	allow_error = value;
}
void slim::common::log::error(const Message message) {
	print(red("ERROR => "), message);
}
void slim::common::log::info(bool value) {
	allow_info = value;
}
void slim::common::log::info(const std::string value_string) {
	std::lock_guard<std::mutex> lock(cerr_mutex);
	std::cerr << value_string << "\n";
}
void slim::common::log::info(const Message message) {
	if(allow_info)
		print(white("INFO => "), message);
}
void slim::common::log::trace(bool value) {
	allow_trace = value;
}
void slim::common::log::trace(const Message message) {
	if(allow_trace)
		print(cyan("TRACE => "), message);
}
void slim::common::log::typescript_warning(bool value) {
	allow_typescript_warning = value;
}
void slim::common::log::typescript_warning(const Message message) {
	if(allow_typescript_warning)
		print(yellow("TS WARNING => "), message);
}
void slim::common::log::print(const std::string log_level, const Message message) {
	if(message.call == nullptr) {
		std::lock_guard<std::mutex> lock(cerr_mutex);
		std::cerr << log_level << blue(std::to_string(message.line_number).c_str()) << ":" << purple(message.file) << ":" << white(message.text) << "\n";
	}
	else {
		std::lock_guard<std::mutex> lock(cerr_mutex);
		std::cerr << log_level << blue(std::to_string(message.line_number).c_str()) << ":" << purple(message.file) << ":" << green(message.call) << ":" << white(message.text) << "\n";
	}
}