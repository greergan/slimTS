#ifndef __SLIM__QUEUE__H
#define __SLIM__QUEUE__H
#include <atomic>
#include <mutex>
#include <string>
#include <queue>
#include <vector>
namespace slim::queue {
	extern std::mutex queue_mutex;
	struct file_storage {
		std::string file_name_string;
		std::string storage_container_handle;
	};
	struct job {
		std::string queue_name_string;
		file_storage ingress_job_file;
		file_storage egress_job_file;
		std::vector<std::string> errors;
		std::atomic<bool> completed = false;
		std::atomic<bool> ready_to_process = false;
		std::atomic<bool> submitted = false;
		bool errored = false;
		job();
		job(std::string queue_name_string, std::string source_storage_handle, std::string source_file_name_string);
		void set_ticket_id(long new_ticket_id);
		long get_ticket_id();
		private:
			long ticket_id;
	};
	job* get_job(std::string queue_name_string);
	job* find_job(std::string queue_name_string, long job_ticket_id);
	void submit(job* job_object);
}
#endif