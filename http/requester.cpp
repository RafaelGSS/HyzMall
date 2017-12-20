#include "../stdafx.h"
#include "requester.h"

using namespace http;

requester::requester()
{
	running = false;
}


requester::~requester()
{
}

void requester::run_thread()
{
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
	while (true)
	{
		check_version();
		//update_tasks();
		//send_task_os();

	}
}

void requester::run()
{
	run_thread();
}


requester* requester::get()
{
	static requester* singleton = nullptr;
	if (!singleton)
		singleton = new requester();
	return singleton;
}