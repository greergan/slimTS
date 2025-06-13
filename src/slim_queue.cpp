#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>
#include <slim/common/log.h>
#include <slim/queue/queue.h>
namespace slim::queue {
	using namespace slim::common;
	static std::unordered_map<std::string, std::vector<job*>> queues;
	static std::atomic<long> ticket_id(0);
	static std::mutex queue_mutex;
	static unsigned int sleep_time = 1;  // next try with std::conditional_variable
}
slim::queue::job::job(){}
slim::queue::job::job(std::string source_storage_handle, std::string source_file_name_string) {
	ingress_job_file.storage_container_handle = source_storage_handle;
	ingress_job_file.file_name_string = source_file_name_string;
}
long slim::queue::job::get_ticket_id() {
	return ticket_id;
}
void slim::queue::job::set_ticket_id(long new_ticket_id) {
	this->ticket_id = new_ticket_id;
}
slim::queue::job* slim::queue::get_job(std::string queue_name_string) {
	log::trace(log::Message("slim::queue::get_job()",std::string("begins => " + queue_name_string).c_str(),__FILE__, __LINE__));
	job* returning_job_object;
	bool job_found = false;
	enter_for:
	std::unique_lock<std::mutex> lock(queue_mutex);
	for(auto* job_object : queues[queue_name_string]) {
		if(job_object->ready_to_process && !job_object->submitted) {
			log::debug(log::Message("slim::queue::get_job()",std::string("job_object->ready_to_process => " + std::to_string(job_object->ready_to_process)).c_str(),__FILE__, __LINE__));
			job_object->set_ticket_id(++ticket_id);
			job_object->submitted = true;
			returning_job_object = job_object;
			job_found = true;
			break;
		}
	}
	lock.unlock();
	if(!job_found) {
		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
		goto enter_for;
	}
	log::trace(log::Message("slim::queue::get_job()",std::string("ends => " + queue_name_string).c_str(),__FILE__, __LINE__));
	return returning_job_object;
}
void slim::queue::submit(job* job_object, std::string queue_name_string) {
	log::trace(log::Message("slim::queue::submit()",std::string("begins => " + queue_name_string).c_str(),__FILE__, __LINE__));
	log::trace(log::Message("slim::queue::submit()",std::string("begins => " + job_object->ingress_job_file.storage_container_handle).c_str(),__FILE__, __LINE__));
	log::trace(log::Message("slim::queue::submit()",std::string("begins => " + job_object->ingress_job_file.file_name_string).c_str(),__FILE__, __LINE__));
	std::unique_lock<std::mutex> lock(queue_mutex);
	queues[queue_name_string].emplace_back(job_object);
	job_object->ready_to_process = true;
	lock.unlock();
	while(!job_object->completed){
		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
	}
	lock.lock();
	for(std::vector<job*>::iterator job_iterator = queues[queue_name_string].begin(); job_iterator != queues[queue_name_string].end();) {
		job* temp_job = *job_iterator;
		if(job_object->get_ticket_id() == temp_job->get_ticket_id()) {
			queues[queue_name_string].erase(job_iterator);
		}
		else {
			job_iterator++;
		}
	}
	lock.unlock();
	log::trace(log::Message("slim::queue::submit()",std::string("ends => " + queue_name_string).c_str(),__FILE__, __LINE__));
	log::trace(log::Message("slim::queue::submit()",std::string("ends => " + job_object->ingress_job_file.storage_container_handle).c_str(),__FILE__, __LINE__));
	log::trace(log::Message("slim::queue::submit()",std::string("ends => " + job_object->ingress_job_file.file_name_string).c_str(),__FILE__, __LINE__));
}