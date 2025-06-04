#ifndef __SLIM__COMMON__LOG__HPP
#define __SLIM__COMMON__LOG__HPP
#include <cstring>
#include <map>
#include <string>

namespace slim::common::log {
	static bool allow_debug = false;
	static bool allow_error = false;
	static bool allow_info = false;
	static bool allow_trace = false;
	static bool allow_typescript_warning = false;
	struct Message {
		char* call = nullptr;
		char* file = nullptr;
		char* text = nullptr;
		int line_number;
		int code;
		Message(const char* call, const char* text, const char* file, const int line_number)
			: call(const_cast<char*>(call)), text(const_cast<char*>(text)), file(const_cast<char*>(file)), line_number(line_number) {}
		Message(const char* text, const char* file, const int line_number)
			: text(const_cast<char*>(text)), file(const_cast<char*>(file)), line_number(line_number) {}
		Message(const std::string text, const char* file, const int line_number)
			: text(const_cast<char*>(text.c_str())), file(const_cast<char*>(file)), line_number(line_number) {}
	};
	void all(const bool value);
	void print(const std::string log_level, const slim::common::log::Message message);
	void error(const bool value);
	void error(const slim::common::log::Message message);
	void debug(const bool value);
	void debug(const slim::common::log::Message message);
	void info(const bool value);
	void info(const std::string value_string);
	void info(const slim::common::log::Message message);
	void trace(const bool value);
	void trace(const slim::common::log::Message message);
	void typescript_warning(const bool value);
	void typescript_warning(const slim::common::log::Message message);
}
#endif