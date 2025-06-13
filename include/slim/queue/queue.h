#ifndef __SLIM__QUEUE__H
#define __SLIM__QUEUE__H
#include <atomic>
#include <memory>
#include <string>
#include <queue>
#include <vector>
namespace slim::queue {
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
		job(std::string source_storage_handle, std::string source_file_name_string);
		void set_ticket_id(long new_ticket_id);
		long get_ticket_id();
		private:
			long ticket_id;
	};
	void complete_job(job);
	job* get_job(std::string queue_name_string);
	void submit(job* job_object, std::string queue_name_string);
}
#endif