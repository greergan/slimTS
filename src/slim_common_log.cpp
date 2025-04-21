#include <iostream>
#include <slim/common/log.h>

static std::map<std::string, std::string> colors {
	{"cyan", "\033[0;36m"},
 	{"red", "\033[0;31m"},
	{"yellow", "\033[0;33m"},
	{"white", "\033[1;37m"}
};
static std::string colorize(const char* input, const char* color) {
	return std::string(colors[const_cast<char*>(color)]) + std::string(input) + "\033[0m";
}
static std::string cyan(const char* input) {
	return colorize(input, "red");
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

void slim::common::log::debug(bool value) {
	slim::common::log::allow_debug = value;
}
void slim::common::log::debug(const slim::common::log::Message message) {
	if(slim::common::log::allow_debug)
		slim::common::log::print(cyan("DEBUG => "), message);
}
void slim::common::log::error(bool value) {
	slim::common::log::allow_error = value;
}
void slim::common::log::error(const slim::common::log::Message message) {
	if(slim::common::log::allow_error)
		slim::common::log::print(red("ERROR => "), message);
}
void slim::common::log::info(bool value) {
	slim::common::log::allow_info = value;
}
void slim::common::log::info(const slim::common::log::Message message) {
	if(slim::common::log::allow_info)
		slim::common::log::print(white("INFO => "), message);
}
void slim::common::log::trace(bool value) {
	slim::common::log::allow_trace = value;
}
void slim::common::log::trace(const slim::common::log::Message message) {
	if(slim::common::log::allow_trace)
		slim::common::log::print(yellow("TRACE => "), message);
}
void slim::common::log::print(const std::string log_level, const slim::common::log::Message message) {
	if(message.call == nullptr)
		std::cout << log_level << std::to_string(message.line_number) << ":" << message.file << ":" << message.text << std::endl;
	else
		std::cout << log_level << std::to_string(message.line_number) << ":" << message.file << ":" << message.call << ":" << message.text << std::endl;
}