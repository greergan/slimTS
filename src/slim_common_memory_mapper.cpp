#include <slim/common/memory_mapper.h>
#include <slim/common/log.h>
using namespace slim::common;
void slim::common::memory_mapper::attach(std::string pipe_name_string, pipe_map_pointer file_map) {
	log::trace(log::Message("slim::common::memory_mapper::attach()", std::string("begins =>" + pipe_name_string).c_str(), __FILE__, __LINE__));
	pipes[pipe_name_string] = file_map;
	log::trace(log::Message("slim::common::memory_mapper::attach()", std::string("ends =>" + pipe_name_string).c_str(), __FILE__, __LINE__));
}
void slim::common::memory_mapper::delete_unordered_map(std::string pipe_name_string) {
	log::trace(log::Message("slim::common::memory_mapper::delete_unordered_map()", std::string("begins =>" + pipe_name_string).c_str(), __FILE__, __LINE__));
	if(pipes.contains(pipe_name_string)) {
		pipes.erase(pipe_name_string);
	}
	log::trace(log::Message("slim::common::memory_mapper::delete_unordered_map()", std::string("ends =>" + pipe_name_string).c_str(), __FILE__, __LINE__));
}
std::shared_ptr<std::string> slim::common::memory_mapper::read(std::string pipe_name_string, std::string file_name_string) {
	if(pipes.contains(pipe_name_string)) {
		if(pipes[pipe_name_string].get()->contains(file_name_string)) {
			return pipes[pipe_name_string].get()->find(file_name_string)->second;
		}
	}
	return std::make_shared<std::string>();
}
void slim::common::memory_mapper::write(std::string pipe_name_string, std::string file_name_string, std::string content_string) {
	if(pipes.contains(pipe_name_string)) {
		pipes[pipe_name_string].get()->emplace(file_name_string, std::make_unique<std::string>(content_string));
	}
}