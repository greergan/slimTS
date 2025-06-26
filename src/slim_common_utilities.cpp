#include <algorithm>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <slim/common/utilities.h>
bool slim::common::utilities::get_bool_value(char* value) {
	if(value) {
		std::string string_value(value);
		std::transform(string_value.begin(), string_value.end(), string_value.begin(), ::tolower);
		if(string_value == "true") {
			return true;
		}
        try {
            return std::stoi(std::string(value));
        }
        catch (std::invalid_argument const& ex) {
            return false;
        }
        catch (std::out_of_range const& ex) {
			return false;
        }
	}
	return false;
}
int slim::common::utilities::get_int_value(char* value) {
	std::string string_value{value};
	return get_int_value(string_value);
}
int slim::common::utilities::get_int_value(std::string* value) {
	std::string string_value{value->c_str()};
	return get_int_value(string_value);
}
int slim::common::utilities::get_int_value(std::string value) {
	if(!value.empty()) {
        try {
            return std::stoi(value);
        }
        catch (std::invalid_argument const& ex) {
            return -1;
        }
        catch (std::out_of_range const& ex) {
			return -1;
        }
	}
	return -1;
}
