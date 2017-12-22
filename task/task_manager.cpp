#include "task_manager.h"


task_manager::task_manager()
{
	running = false;
	// TODO - setar um callback on_execute para cada vez que executar a task eliminar do banco de dados task_user_id

}


task_manager::~task_manager()
{
}

void task_manager::add_task(std::shared_ptr<task_info> _new_task)
{
	if (check_new_task(_new_task)) {
		// TODO - MUTEX AQUI
		tasks.push_back(_new_task);
	}

}

task_manager* task_manager::get()
{
	static task_manager* singleton = nullptr;
	if (!singleton)
		singleton = new task_manager();

	return singleton;

}

bool task_manager::check_new_task(std::shared_ptr<task_info> _new_task)
{
	auto function_find = [&](std::shared_ptr<task_info> task){
		return task->id == _new_task->id;
	};

	auto _found = std::find_if(
			tasks.begin(),
			tasks.end(),
			function_find
		);

	if (_found == tasks.end())
		return false;

	return true;
}

task_info task_manager::get_next_task()
{
	// TODO - mutex aqui

	auto current_task = tasks.begin();
	tasks.erase(tasks.begin());

	return current_task;
}

void task_manager::run()
{
	start_thread();
}

void task_manager::start_thread()
{
	if (running)
		return;

	running = true;

	std::thread(
		std::bind(
			&runner_thread,
			this
		)
	).detach();
}

void task_manager::runner_thread()
{
	while (true)
	{
		if (tasks.size())
		{
			auto task = get_next_task();
			start_task(task);
		}

		boost::this_thread::sleep_for(
			boost::chrono::milliseconds(
				5000
			)
		);

	}
}

void task_manager::start_task(task_info task)
{
	if(task.type == "ssh")
	{
		/*task_ssh new_task;
		std::thread(
			std::bind(
				&task_ssh::run, task_ssh
			)
		).detach();
		return;
		*/
	}
}