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

void requester::runner_thread()
{
	while (true)
	{
		//check_version();
		//update_tasks();
		//send_task_os();

	}
}

void requester::run()
{
	run_thread();
}
