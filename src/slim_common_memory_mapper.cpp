#include <unordered_map>
#include <slim/common/memory_mapper.h>
#include <slim/common/log.h>
using namespace slim::common;
void slim::common::memory_mapper::attach(std::string pipe_name_string) {
	log::trace(log::Message("slim::common::memory_mapper::attach()", std::string("begins => " + pipe_name_string).c_str(), __FILE__, __LINE__));
	if(!pipes.contains(pipe_name_string)) {
		create(pipe_name_string);
	}
	log::trace(log::Message("slim::common::memory_mapper::attach()", std::string("ends => " + pipe_name_string).c_str(), __FILE__, __LINE__));
}
void slim::common::memory_mapper::attach(std::string pipe_name_string, pipe_map_pointer file_map) {
	log::trace(log::Message("slim::common::memory_mapper::attach()", std::string("begins file_map => " + pipe_name_string).c_str(), __FILE__, __LINE__));
	pipes[pipe_name_string] = file_map;
	log::trace(log::Message("slim::common::memory_mapper::attach()", std::string("ends => file_map" + pipe_name_string).c_str(), __FILE__, __LINE__));
}
void slim::common::memory_mapper::create(std::string pipe_name_string) {
	log::trace(log::Message("slim::common::memory_mapper::create()", std::string("begins => " + pipe_name_string).c_str(), __FILE__, __LINE__));
		using content_pointer = std::shared_ptr<std::string>;
	using map_container = std::unordered_map<std::string, content_pointer>;
	using pipe_map_pointer = std::shared_ptr<map_container>;
	if(!pipes.contains(pipe_name_string)) {
		pipes[pipe_name_string] = std::make_shared<map_container>(map_container());
	}
	log::trace(log::Message("slim::common::memory_mapper::create()", std::string("ends => " + pipe_name_string).c_str(), __FILE__, __LINE__));
}
void slim::common::memory_mapper::delete_unordered_map(std::string pipe_name_string) {
	log::trace(log::Message("slim::common::memory_mapper::delete_unordered_map()", std::string("begins =>" + pipe_name_string).c_str(), __FILE__, __LINE__));
	if(pipes.contains(pipe_name_string)) {
		pipes.erase(pipe_name_string);
	}
	log::trace(log::Message("slim::common::memory_mapper::delete_unordered_map()", std::string("ends =>" + pipe_name_string).c_str(), __FILE__, __LINE__));
}
bool slim::common::memory_mapper::exists(std::string pipe_name_string, std::string file_name_string) {
	log::trace(log::Message("slim::common::memory_mapper::exists()", std::string("begins => " + pipe_name_string).c_str(), __FILE__, __LINE__));
	log::trace(log::Message("slim::common::memory_mapper::exists()", std::string("begins => " + file_name_string).c_str(), __FILE__, __LINE__));
	log::debug(log::Message("slim::common::memory_mapper::exists()", std::string("pipes.contains() => " + std::to_string(pipes.contains(pipe_name_string))).c_str(), __FILE__, __LINE__));
	bool return_value = false;
	if(pipes.contains(pipe_name_string)) {
		log::debug(log::Message("slim::common::memory_mapper::exists()", std::string("pipes[pipe_name_string].get()->contains() => " + std::to_string(pipes[pipe_name_string].get()->contains(file_name_string))).c_str(), __FILE__, __LINE__));
		return_value = pipes[pipe_name_string].get()->contains(file_name_string);
	}
	log::trace(log::Message("slim::common::memory_mapper::exists()", std::string("ends => " + pipe_name_string).c_str(), __FILE__, __LINE__));
	log::trace(log::Message("slim::common::memory_mapper::exists()", std::string("ends => " + file_name_string).c_str(), __FILE__, __LINE__));
	return return_value;
}
std::shared_ptr<std::string> slim::common::memory_mapper::read(std::string pipe_name_string, std::string file_name_string) {
	log::trace(log::Message("slim::common::memory_mapper::read()", std::string("begins => " + pipe_name_string).c_str(), __FILE__, __LINE__));
	log::trace(log::Message("slim::common::memory_mapper::read()", std::string("begins => " + file_name_string).c_str(), __FILE__, __LINE__));
	if(pipes.contains(pipe_name_string)) {
		if(pipes[pipe_name_string].get()->contains(file_name_string)) {
			return pipes[pipe_name_string].get()->find(file_name_string)->second;
		}
		else {
			log::error(log::Message("slim::common::memory_mapper::read()", std::string("not found => " + file_name_string).c_str(), __FILE__, __LINE__));
		}
	}
	else {
		log::error(log::Message("slim::common::memory_mapper::read()", std::string("not found => " + pipe_name_string).c_str(), __FILE__, __LINE__));
	}
	log::trace(log::Message("slim::common::memory_mapper::read()", std::string("ends => " + pipe_name_string).c_str(), __FILE__, __LINE__));
	log::trace(log::Message("slim::common::memory_mapper::read()", std::string("ends => " + file_name_string).c_str(), __FILE__, __LINE__));
	return std::make_shared<std::string>();
}
void slim::common::memory_mapper::register_path(std::string pipe_name_string, std::string path_in) {
	log::trace(log::Message("slim::common::memory_mapper::register_path()", std::string("begins => " + pipe_name_string).c_str(), __FILE__, __LINE__));
	log::trace(log::Message("slim::common::memory_mapper::register_path()", std::string("begins => " + path_in).c_str(), __FILE__, __LINE__));
	if(!pipes.contains(pipe_name_string)) {
		create(pipe_name_string);
	}
	if(pipes.contains(pipe_name_string)) {
		auto path = path_in;
		pipes[pipe_name_string].get()->emplace(path, std::make_unique<std::string>(""));
		int position = 0;
		while((position = path.find_last_of('/')) != std::string::npos) {
			path = path.substr(0, position);
			pipes[pipe_name_string].get()->emplace(path, std::make_unique<std::string>(""));
			if(path.ends_with("://")) {
				break;
			}
			log::debug(log::Message("slim::common::memory_mapper::register_path()", std::string("path part => " + path).c_str(), __FILE__, __LINE__));
		}
	}
	log::trace(log::Message("slim::common::memory_mapper::register_path()", std::string("ends => " + pipe_name_string).c_str(), __FILE__, __LINE__));
	log::trace(log::Message("slim::common::memory_mapper::register_path()", std::string("ends => " + path_in).c_str(), __FILE__, __LINE__));
}
void slim::common::memory_mapper::write(std::string pipe_name_string, std::string file_name_string, std::string content_string) {
	log::trace(log::Message("slim::common::memory_mapper::write()", std::string("begins => " + pipe_name_string).c_str(), __FILE__, __LINE__));
	log::trace(log::Message("slim::common::memory_mapper::write()", std::string("begins => " + file_name_string).c_str(), __FILE__, __LINE__));
	log::debug(log::Message("slim::common::memory_mapper::write()", std::string("content length => " + std::to_string(content_string.size())).c_str(), __FILE__, __LINE__));
	if(pipes.contains(pipe_name_string)) {
		pipes[pipe_name_string].get()->emplace(file_name_string, std::make_unique<std::string>(content_string));
	}
	log::trace(log::Message("slim::common::memory_mapper::write()", std::string("ends => " + pipe_name_string).c_str(), __FILE__, __LINE__));
	log::trace(log::Message("slim::common::memory_mapper::write()", std::string("ends => " + file_name_string).c_str(), __FILE__, __LINE__));
}