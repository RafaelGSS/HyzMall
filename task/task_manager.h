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
	std::string _class;
	std::string method;
	std::vector<std::string> args;
	bool response_up;

	task_info(json_var _record, bool _response_up = false)
	{
		id = _record["id"].dump();
		_class = _record["class_name"].dump();
		method = _record["method"].dump();
		response_up = _response_up;
		args = args_to_vector(_record["args"].dump());
		
		trim_json(id);
		trim_json(_class);
		trim_json(method);

	}
	void trim_json(std::string& str)
	{
		str.erase(
			remove(str.begin(), str.end(), '\"'),
			str.end()
		);

	}

	std::vector<std::string> args_to_vector(std::string args)
	{
		trim_json(args);
		std::vector<std::string> result;
		std::istringstream iss(args);

		for (std::string token; std::getline(iss, token, '|'); )
		{
			result.push_back(std::move(token));
		}

		return result;
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