
#ifndef __SLIM__COMMON__MEMORY_MAPPER__H
#define __SLIM__COMMON__MEMORY_MAPPER__H
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <slim/common/log.h>
namespace slim::common::memory_mapper {
	using namespace slim::common;
	using content_variant = std::variant<std::string, std::shared_ptr<std::string>>;
	using map_container = std::unordered_map<std::string, content_variant>;
	using map_pointer = std::shared_ptr<map_container>;
	static std::unordered_map<std::string, map_pointer> maps;
	void attach(std::string map_name_string, map_pointer map);
	void create(std::string map_name_string);
	void erase(std::string map_name_string);
	void erase(std::string map_name_string, std::string file_name_string);
	bool exists(std::string map_name_string);
	bool exists(std::string map_name_string, std::string file_name_string);
	const std::vector<std::string> list_keys(std::string map_name_string);
	std::shared_ptr<std::string> read(std::string map_name_string, std::string file_name_string);
	std::string read_string(std::string map_name_string, std::string file_name_string);
	void write(std::string map_name_string, std::string file_name_string, content_variant file_content_string_or_pointer);
}
#endif