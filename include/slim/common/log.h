#ifndef __SLIM__COMMON__LOG__H
#define __SLIM__COMMON__LOG__H
#include <cstring>
#include <memory>
#include <sstream>
#include <string>
#include <variant>
namespace slim::common::log {
	using character_types = std::variant<const char*,char*,std::string>;
	struct Message {
		int line_number = -1;
		std::string call;
		std::string text;
		std::string file;
		Message() = delete;
		Message(character_types call, character_types text, character_types file, int line_number);
	};
	void print_all(bool value);
	void print_color(bool value);
	void print_debug(bool value);
	void print_error(bool value);
	void print_info(bool value);
	void print_thread_identity(bool value);
	void print_trace(bool value);
	void print_warn(bool value);
	void error(Message message);
	void debug(Message message);
	void info(Message message);
	void info(character_types value_string);
	void trace(Message message);
	void warn(Message message);
	void print(std::string log_level,  Message message, bool is_error = false);
}
#endif