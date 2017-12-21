#pragma once
#include <vector>
#include <memory>
#include <algorithm>

struct task_info {
	task_info() {

	}
};

class task_manager
{
	static std::vector<std::shared_ptr<task_info>> tasks;
public:
	task_manager();
	~task_manager();
	bool check_new_task(std::shared_ptr<task_info>);
	void add_task(std::shared_ptr<task_info>);
	void run();
	static task_manager* get();
};

