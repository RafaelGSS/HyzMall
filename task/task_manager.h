#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include <functional>
#include <mutex>
#include <thread>
	#include <iostream>
#include "../http/json/json.hpp"
#include <chrono>
#include <task\base\tasker.h>
#include <condition_variable>

using json_var = nlohmann::json;  

struct task_info {
public:
	std::string id;
	std::string type;
	std::string args_type;
	bool response_up;

	task_info(json_var _record, bool _response_up = false)
	{
		id = _record["id"].dump();
		type = _record["type"].dump();
		response_up = _response_up;
		args_type = _record["args_type"].dump();
		
		trim_json(args_type);
		trim_json(type);
		trim_json(id);

	}
	void* trim_json(std::string& str)
	{
		str.erase(
			remove(str.begin(), str.end(), '\"'),
			str.end()
		);

	}
};

class task_manager
{
	std::mutex mtx;
	std::vector<std::shared_ptr<task_info>> tasks;
	std::vector<std::shared_ptr<task_info>> ret_tasks;
	std::condition_variable cv;
	bool running;
public:
	task_manager();
	~task_manager();
	bool check_new_task(std::shared_ptr<task_info>);
	bool wait_at_task();
	bool check_tasks();
	void add_task(std::shared_ptr<task_info>);
	void run();
	void start_thread();
	void runner_thread();
	void start_task(std::shared_ptr<task_info>);
	std::shared_ptr<task_info> get_next_task();
	static task_manager* get();
};

