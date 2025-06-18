#include <concepts>
#include <type_traits>
#include <unordered_map>
#include <slim/common/memory_mapper.h>
#include <slim/common/log.h>
namespace slim::common::memory_mapper {
	using namespace slim::common;
}
void slim::common::memory_mapper::attach(std::string pipe_name_string, pipe_map_pointer map) {
	log::trace(log::Message("slim::common::memory_mapper::attach()", std::string("begins => " + pipe_name_string).c_str(), __FILE__, __LINE__));
	if(map == nullptr) {
		log::error(log::Message("slim::common::memory_mapper::create()", std::string("map == nullptr").c_str(), __FILE__, __LINE__));
		throw("slim::common::memory_mapper::attach() nullptr exception");
	}
	try {	
		if(!exists(pipe_name_string)) {
			pipes[pipe_name_string] = map;
			log::debug(log::Message("slim::common::memory_mapper::attach()", std::string("attached => " + pipe_name_string).c_str(), __FILE__, __LINE__));
			log::debug(log::Message("slim::common::memory_mapper::attach()", std::string("slot size => " + std::to_string(map->size())).c_str(), __FILE__, __LINE__));
		}
		else {
			log::debug(log::Message("slim::common::memory_mapper::attach()", std::string("did not attach to existing slot => " + pipe_name_string).c_str(), __FILE__, __LINE__));
		}
	}
	catch(const std::bad_alloc& e) {
		throw(e);
	}
	log::trace(log::Message("slim::common::memory_mapper::attach()", std::string("ends => " + pipe_name_string).c_str(), __FILE__, __LINE__));
}
void slim::common::memory_mapper::create(std::string pipe_name_string) {
	log::trace(log::Message("slim::common::memory_mapper::create()", std::string("begins => " + pipe_name_string).c_str(), __FILE__, __LINE__));
	using content_pointer = std::shared_ptr<std::string>;
	using map_container = std::unordered_map<std::string, content_pointer>;
	using pipe_map_pointer = std::shared_ptr<map_container>;
	try {
		if(!exists(pipe_name_string)) {
			pipes[pipe_name_string] = std::make_shared<map_container>(map_container());
			log::debug(log::Message("slim::common::memory_mapper::create()", std::string("created => " + pipe_name_string).c_str(), __FILE__, __LINE__));
		}
	}
	catch(const std::bad_alloc& e) {
		throw(e);
	}
	log::trace(log::Message("slim::common::memory_mapper::create()", std::string("ends => " + pipe_name_string).c_str(), __FILE__, __LINE__));
}
void slim::common::memory_mapper::create_path(std::string pipe_name_string, std::string path_in) {
	log::trace(log::Message("slim::common::memory_mapper::create_path()", std::string("begins => " + pipe_name_string).c_str(), __FILE__, __LINE__));
	log::trace(log::Message("slim::common::memory_mapper::create_path()", std::string("begins => " + path_in).c_str(), __FILE__, __LINE__));
	if(!exists(pipe_name_string)) {
		create(pipe_name_string);
	}
	auto path = path_in;
	pipes[pipe_name_string].get()->emplace(path, std::make_shared<std::string>(""));
	int position = 0;
	while((position = path.find_last_of('/')) != std::string::npos) {
		path = path.substr(0, position);
		pipes[pipe_name_string].get()->emplace(path, std::make_shared<std::string>(""));
		if(path.ends_with("://")) {
			break;
		}
		log::debug(log::Message("slim::common::memory_mapper::create_path()", std::string("path part => " + path).c_str(), __FILE__, __LINE__));
	}
	log::trace(log::Message("slim::common::memory_mapper::create_path()", std::string("ends => " + pipe_name_string).c_str(), __FILE__, __LINE__));
	log::trace(log::Message("slim::common::memory_mapper::create_path()", std::string("ends => " + path_in).c_str(), __FILE__, __LINE__));
}
void slim::common::memory_mapper::erase(std::string pipe_name_string) {
	log::trace(log::Message("slim::common::memory_mapper::delete_unordered_map()", std::string("begins =>" + pipe_name_string).c_str(), __FILE__, __LINE__));
	if(exists(pipe_name_string)) {
		pipes.erase(pipe_name_string);
	}
	log::trace(log::Message("slim::common::memory_mapper::delete_unordered_map()", std::string("ends =>" + pipe_name_string).c_str(), __FILE__, __LINE__));
}
bool slim::common::memory_mapper::exists(std::string pipe_name_string) {
	log::trace(log::Message("slim::common::memory_mapper::exists()", std::string("begins => " + pipe_name_string).c_str(), __FILE__, __LINE__));
	bool return_value = pipes.count(pipe_name_string) > 0 ? true : false;
	std::string debug_message = return_value ? "slot found => " : "slot not found => ";
	log::debug(log::Message("slim::common::memory_mapper::exists()", std::string(debug_message + pipe_name_string).c_str(), __FILE__, __LINE__));
	log::trace(log::Message("slim::common::memory_mapper::exists()", std::string("ends => " + pipe_name_string).c_str(), __FILE__, __LINE__));
	return return_value;
}
bool slim::common::memory_mapper::exists(std::string pipe_name_string, std::string file_name_string) {
	log::trace(log::Message("slim::common::memory_mapper::exists()", std::string("begins => " + pipe_name_string).c_str(), __FILE__, __LINE__));
	log::trace(log::Message("slim::common::memory_mapper::exists()", std::string("begins => " + file_name_string).c_str(), __FILE__, __LINE__));
	bool return_value = false;
	if(exists(pipe_name_string)) {
		log::debug(log::Message("slim::common::memory_mapper::exists()", std::string("slot found => " + pipe_name_string).c_str(), __FILE__, __LINE__));
		auto pipe_map = pipes[pipe_name_string];
		log::debug(log::Message("slim::common::memory_mapper::exists()", std::string("slot size => " + std::to_string(pipe_map->size())).c_str(), __FILE__, __LINE__));
		return_value = pipes[pipe_name_string].get()->count(file_name_string) > 0 ? true : false;
		std::string found_message_string = return_value == true ? "found => " : "did not find => ";
		log::debug(log::Message("slim::common::memory_mapper::exists()", std::string(found_message_string + pipe_name_string).c_str(), __FILE__, __LINE__));
	}
	else {
		log::debug(log::Message("slim::common::memory_mapper::exists()", std::string("slot not found => " + pipe_name_string).c_str(), __FILE__, __LINE__));
	}
	log::trace(log::Message("slim::common::memory_mapper::exists()", std::string("ends => " + pipe_name_string).c_str(), __FILE__, __LINE__));
	log::trace(log::Message("slim::common::memory_mapper::exists()", std::string("ends => " + file_name_string).c_str(), __FILE__, __LINE__));
	return return_value;
}
const std::vector<std::string> slim::common::memory_mapper::list_keys(std::string pipe_name_string) {
	log::trace(log::Message("slim::common::memory_mapper::list_keys()", std::string("begins => " + pipe_name_string).c_str(), __FILE__, __LINE__));
	std::vector<std::string> keys_vector;
	if(exists(pipe_name_string)) {
		for(auto [key,value] : *pipes[pipe_name_string]) {
			keys_vector.emplace_back(key);
		}
	}
	return keys_vector;
	log::trace(log::Message("slim::common::memory_mapper::list_keys()", std::string("ends => " + pipe_name_string).c_str(), __FILE__, __LINE__));
}
std::shared_ptr<std::string> slim::common::memory_mapper::read(std::string pipe_name_string, std::string file_name_string) {
	log::trace(log::Message("slim::common::memory_mapper::read()", std::string("begins => " + pipe_name_string).c_str(), __FILE__, __LINE__));
	log::trace(log::Message("slim::common::memory_mapper::read()", std::string("begins => " + file_name_string).c_str(), __FILE__, __LINE__));
	if(exists(pipe_name_string)) {
		log::debug(log::Message("slim::common::memory_mapper::read()", std::string("slot found => " + pipe_name_string).c_str(), __FILE__, __LINE__));
		if(exists(pipe_name_string, file_name_string)) {
			auto file_content_pointer = pipes[pipe_name_string].get()->find(file_name_string)->second;
			log::debug(log::Message("slim::common::memory_mapper::read()", std::string("found => " + file_name_string).c_str(), __FILE__, __LINE__));
			log::debug(log::Message("slim::common::memory_mapper::read()",
					std::string("file size => " + std::to_string(file_content_pointer->size())).c_str(), __FILE__, __LINE__));
			log::trace(log::Message("slim::common::memory_mapper::read()", std::string("ends => " + pipe_name_string).c_str(), __FILE__, __LINE__));
			log::trace(log::Message("slim::common::memory_mapper::read()", std::string("ends => " + file_name_string).c_str(), __FILE__, __LINE__));
			return pipes[pipe_name_string].get()->find(file_name_string)->second;
		}
		else {
			log::error(log::Message("slim::common::memory_mapper::read()", std::string("not found => " + file_name_string).c_str(), __FILE__, __LINE__));
		}
	}
	else {
		log::error(log::Message("slim::common::memory_mapper::read()", std::string("slot not found => " + pipe_name_string).c_str(), __FILE__, __LINE__));
	}
	log::trace(log::Message("slim::common::memory_mapper::read()", std::string("ends => " + pipe_name_string).c_str(), __FILE__, __LINE__));
	log::trace(log::Message("slim::common::memory_mapper::read()", std::string("ends => " + file_name_string).c_str(), __FILE__, __LINE__));
	return std::make_shared<std::string>();
}
void slim::common::memory_mapper::write(std::string pipe_name_string, std::string file_name_string, std::shared_ptr<std::string> content_string_pointer) {
	log::trace(log::Message("slim::common::memory_mapper::write()", std::string("begins => " + pipe_name_string).c_str(), __FILE__, __LINE__));
	log::trace(log::Message("slim::common::memory_mapper::write()", std::string("begins => " + file_name_string).c_str(), __FILE__, __LINE__));
	log::debug(log::Message("slim::common::memory_mapper::write()", std::string("file size => " + std::to_string(content_string_pointer->size())).c_str(), __FILE__, __LINE__));
	try {
		if(!exists(pipe_name_string)) {
			create(pipe_name_string);
		}
		pipes[pipe_name_string].get()->emplace(file_name_string, content_string_pointer);
		log::debug(log::Message("slim::common::memory_mapper::write()", std::string("bytes written => " + std::to_string(content_string_pointer->size())).c_str(), __FILE__, __LINE__));
	}
	catch(const std::bad_alloc& e) {
		throw(e);
	}
	log::trace(log::Message("slim::common::memory_mapper::write()", std::string("ends => " + pipe_name_string).c_str(), __FILE__, __LINE__));
	log::trace(log::Message("slim::common::memory_mapper::write()", std::string("ends => " + file_name_string).c_str(), __FILE__, __LINE__));
}