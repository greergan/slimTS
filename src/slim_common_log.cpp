#include <iostream>
#include <slim/common/log.h>

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
	slim::common::log::allow_debug = value;
	slim::common::log::allow_error = value;
	slim::common::log::allow_info = value;
	slim::common::log::allow_trace = value;
}
void slim::common::log::debug(bool value) {
	slim::common::log::allow_debug = value;
}
void slim::common::log::debug(const slim::common::log::Message message) {
	if(slim::common::log::allow_debug)
		slim::common::log::print(yellow("DEBUG => "), message);
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
		slim::common::log::print(cyan("TRACE => "), message);
}
void slim::common::log::print(const std::string log_level, const slim::common::log::Message message) {
	if(message.call == nullptr)
		std::cout << log_level << blue(std::to_string(message.line_number).c_str()) << ":" << purple(message.file) << ":" << white(message.text) << std::endl;
	else
		std::cout << log_level << blue(std::to_string(message.line_number).c_str()) << ":" << purple(message.file) << ":" << green(message.call) << ":" << white(message.text) << std::endl;
}