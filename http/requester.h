#pragma once
#include "http_request.h"
#include <thread>
#include <chrono>
#include <functional>
#include <vector>
#include <mutex>
#include <windows.h>

#include <task/task_manager.h>
#include <version.h>

namespace http {
	class requester : public http_request
	{
	private:
		//std::vector<tasks> task_list;
		bool running;
		std::string client_path;

		std::string get_active_window();
	public:
		requester();
		~requester();
		void run_thread();
		void run();
		void runner_thread();
		void check_version();
		void update_last_login();
		void update_tasks();
		void update_tasks_all();
		void add_task(std::shared_ptr<task_info> new_task);
		static requester* get();
	};
}

