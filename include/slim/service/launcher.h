#ifndef __SLIM__SERVICE__LAUNCHER__H
#define __SLIM__SERVICE__LAUNCHER__H
#include <string>
#include <vector>
#include <slim/module/import_specifier.h>
namespace slim::service::launcher {
	void marshal_resources(std::vector<std::string> v8_configuration_values);
	void launch(slim::module::variant_specifier script_name_string_or_file_definition_struct);
	void tear_down();
}
#endif