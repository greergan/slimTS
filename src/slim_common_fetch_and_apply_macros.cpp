#include <filesystem>
#include <sstream>
#include <string>
#include <slim/common/fetch.h>
#include <slim/common/fetch_and_apply_macros.h>
#include <slim/common/log.h>
#include <slim/macros.h>
std::string slim::common::fetch_and_apply_macros(const char* file_name) {
	slim::common::log::trace(slim::common::log::Message("slim::common::fetch_and_apply_macros()","begins",__FILE__, __LINE__));
	std::filesystem::path absolute_path_to_file = std::filesystem::absolute(file_name);
	std::stringstream script_source_file_contents_stream = slim::common::fetch::fetch(absolute_path_to_file.string().c_str());
	slim::common::log::trace(slim::common::log::Message("slim::common::fetch_and_apply_macros()","called slim::common::fetch::fetch()",__FILE__, __LINE__));
	slim::common::log::trace(slim::common::log::Message("slim::common::fetch_and_apply_macros()","calling slim::macros::apply()",__FILE__, __LINE__));
	std::string source_file_contents = slim::macros::apply(script_source_file_contents_stream, absolute_path_to_file);
	slim::common::log::trace(slim::common::log::Message("slim::start","called slim::macros::apply()",__FILE__, __LINE__));
	slim::common::log::trace(slim::common::log::Message("slim::common::fetch_and_apply_macros()","ends",__FILE__, __LINE__));
	return source_file_contents;
}