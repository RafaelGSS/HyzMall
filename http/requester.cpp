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

	// TODO - pegar task por task e verificar qual foi completada e verificar se tem novas tasks
	for (auto rs : res)
	{
		
	}
}

void requester::add_task(std::shared_ptr<task_manager> new_task)
{
	// TODO - new_task->add_task();
}

requester* requester::get()
{
	static requester* singleton = nullptr;
	if (!singleton)
		singleton = new requester();
	return singleton;
}