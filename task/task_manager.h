#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include <functional>
#include <mutex>
#include <thread>
#include "../http/json/json.hpp"
//#include <boost/chrono.hpp>
#include <chrono>

using json_var = nlohmann::json;  

struct task_info {
public:
	std::string id;
	std::string type;

	task_info(json_var _record)
	{
		id = _record["id"].dump();
		type = _record["type"].dump();
	}
};

class task_manager
{
	std::mutex mtx;
	std::vector<std::shared_ptr<task_info>> tasks;
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

