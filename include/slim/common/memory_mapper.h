
#ifndef __SLIM__COMMON__MEMORY_MAPPER__H
#define __SLIM__COMMON__MEMORY_MAPPER__H
#include <memory>
#include <string>
#include <unordered_map>
namespace slim::common::memory_mapper {
	using content_pointer = std::shared_ptr<std::string>;
	using map_container = std::unordered_map<std::string, content_pointer>;
	using pipe_map_pointer = std::shared_ptr<map_container>;
	static std::unordered_map<std::string, pipe_map_pointer> pipes;
	void attach(std::string pipe_name_string);
	void attach(std::string pipe_name_string, pipe_map_pointer map);
	void create(std::string pipe_name_string);
	bool exists(std::string pipe_name_string, std::string content_name_string);
	std::shared_ptr<std::string> read(std::string pipe_name_string, std::string content_name_string);
	void register_path(std::string pipe_name_string, std::string path);
	void write(std::string pipe_name_string, std::string content_name_string, std::string content_string);
	void delete_unordered_map(std::string pipe_name_string);
}
#endif