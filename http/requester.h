#pragma once
#include "http_request.h"
#include <thread>
#include <functional>
#include <vector>

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
	};
}

