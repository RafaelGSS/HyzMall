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
		//update_tasks();
		//update_tasks_all();
		//send_task_os();

		std::this_thread::sleep_for(std::chrono::milliseconds(delay_thread));
	}
}

void requester::run(){
	run_thread();
}


void requester::update_tasks()
{
	uint32_t ec = 0;
	auto response = post_request("tasks", get_basic_body().dump(), ec);
	auto res = json_var::parse(response);

	if (ec)
		return;
	
	// TODO - return json to vector
	//	auto _tasks = _value.to_vector < BaseTaskT >();
	/*std::for_each(
		_tasks.begin(),
		_tasks.end(),
		[&](BaseTaskT& _task) {
		auto _new_task = std::shared_ptr < BaseTaskT >(new BaseTaskT(_task));
		new_tasks[_task.get_id()] = _new_task;
		_new_task->set_cb_step(
			std::bind(&base_tasker::on_step, this, std::placeholders::_1)
		);
		_new_task->set_cb_completion(
			std::bind(&base_tasker::on_completion, this, std::placeholders::_1)
		);
	}
	);*/
	

	for (auto rs : res)
	{
		std::shared_ptr<task_info> tasks_tmp(rs);
		add_task(rs);
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