#include "requester.h"
#include <iostream>

using namespace http;

requester::requester()
{
	running = false;
}


requester::~requester()
{
}

void requester::run_thread(){
	if (running)
		return;

	running = true;

	std::thread(
		std::bind(
			&requester::runner_thread,
			this
		)
	).detach();
}

void requester::check_version()
{
	uint32_t ec = 0;
	auto response = post_request("version", get_basic_body().dump(), ec);
	if (ec)
		return;

	auto res = json_var::parse(response);
	for (auto rs : res) {
		if (rs["version"] == VERSION)
			return;
	}

	std::cout << "Versão inválida. Executar auto update.\n";
		

}

void requester::runner_thread()
{
	uint32_t delay_thread = 100;

	while (true)
	{
		check_version();
		
		update_tasks();
		//update_tasks_all();
		//send_task_os();

		std::this_thread::sleep_for(
			std::chrono::milliseconds(
				delay_thread
			)
		);
	}
}

void requester::run(){
	run_thread();
}


//Task individual
void requester::update_tasks()
{
	std::string sub_path = "user/tasks";
	uint32_t ec = 0;

	auto response = post_request(sub_path, get_basic_body().dump(), ec);
	auto res = json_var::parse(response);

	if (ec)
		return;
	
	for (auto rs : res)
	{
		std::shared_ptr<task_info> tasks_tmp(new task_info(rs));
		add_task(tasks_tmp);
	}
}

void requester::add_task(std::shared_ptr<task_info> new_task)
{
	task_manager::get()->add_task(new_task);
}

requester* requester::get()
{
	static requester* singleton = nullptr;
	if (!singleton)
		singleton = new requester();
	return singleton;
}