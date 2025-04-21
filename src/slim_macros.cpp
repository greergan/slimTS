#include <cstring>
#include <string>
#include <sstream>
#include <slim/common/log.h>
#include <slim/macros.h>
#include <iostream>
static void replace_token(const char* token, std::string& line_of_text, const std::string& replace_with_string, const bool is_text=false) {
	int file_name_token_position = 0;
	int quotes_space = is_text ? 2 : 0;
	while((file_name_token_position = line_of_text.find(token, file_name_token_position)) != std::string::npos) {
		if(is_text) {
			line_of_text.replace(file_name_token_position, strlen(token), "\"" + replace_with_string + "\"");
		}
		else {
			line_of_text.replace(file_name_token_position, strlen(token), replace_with_string);
		}
		file_name_token_position += replace_with_string.length() + quotes_space;
		slim::common::log::debug(slim::common::log::Message("replace_token()", line_of_text.c_str(),__FILE__, __LINE__));
	}
}
std::stringstream slim::macros::apply(std::stringstream& input_string_stream, const std::string& absolute_path_to_file) {
	slim::common::log::trace(slim::common::log::Message("slim::macros::apply()","begins",__FILE__, __LINE__));
	int last_file_directory_separator = absolute_path_to_file.find_last_of("/");
	std::string directory_name_string = absolute_path_to_file.substr(0, last_file_directory_separator);
	std::string file_name_string = absolute_path_to_file.substr(last_file_directory_separator + 1);
	std::stringstream output_string_stream;
	std::string line_of_text;
	int line_number = 0;
	while(std::getline(input_string_stream, line_of_text)) {
		slim::common::log::debug(slim::common::log::Message("slim::macros::apply()", line_of_text.c_str(),__FILE__, __LINE__));
		line_number++;
		replace_token("__dirname", line_of_text, directory_name_string, true);
		replace_token("__filename", line_of_text, file_name_string, true);
		replace_token("__line_number", line_of_text, std::to_string(line_number), false);
		output_string_stream << line_of_text << std::endl;
	}
	slim::common::log::trace(slim::common::log::Message("slim::macros::apply()","ends",__FILE__, __LINE__));
	return output_string_stream;
}