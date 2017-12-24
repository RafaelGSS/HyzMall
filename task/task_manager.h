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

using json_var = nlohmann::json;  

struct task_info {
public:
	std::string id;
	std::string type;

	task_info(json_var _record)
	{

		id = _record["id"].dump();
		trim_json(id);

		type = _record["type"].dump();
	}
	void* trim_json(std::string& str)
	{
		str.erase(str.begin());
		str.erase(str.end()-1);
	}
};

class task_manager
{
	std::mutex mtx;
	std::vector<std::shared_ptr<task_info>> tasks;
	std::vector<std::shared_ptr<task_info>> ret_tasks;
	bool running;
public:
	task_manager();
	~task_manager();
	bool check_new_task(std::shared_ptr<task_info>);
	void add_task(std::shared_ptr<task_info>);
	void run();
	void start_thread();
	void runner_thread();
	void start_task(std::shared_ptr<task_info>);
	std::shared_ptr<task_info> get_next_task();
	static task_manager* get();
};

