#pragma once
#include "http_request.h"
#include <thread>
#include <functional>
#include <vector>
#include <mutex>
#include "../task/task_manager.h"
#include "../version.h"

namespace http {
	class requester : public http_request
	{
	private:
		//std::vector<tasks> task_list;
		bool running;
	public:
		requester();
		~requester();
		void run_thread();
		void run();
		void runner_thread();
		void check_version();
		void update_tasks();
		void add_task(std::shared_ptr<task_info> new_task);
		static requester* get();
	};
}

