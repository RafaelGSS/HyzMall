#include "task_manager.h"


task_manager::task_manager()
{
	running = false;
}


task_manager::~task_manager()
{
}

void task_manager::add_task(std::shared_ptr<task_info> _new_task)
{
	std::lock_guard<std::mutex> lck(mtx);

	if (check_new_task(_new_task)) {
		std::cout << "task_manager::added task type " << _new_task->method << std::endl;
		tasks.push_back(_new_task);
		ret_tasks.push_back(_new_task);
		cv.notify_all();
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
	bool _found = true;
	
	auto function_find = [&](std::shared_ptr<task_info> task){
		if (task->id == _new_task->id)
			_found = false;
	};

	std::for_each(
		ret_tasks.begin(),
		ret_tasks.end(),
		function_find
	);

	return _found;
}

std::shared_ptr<task_info> task_manager::get_next_task()
{
	std::lock_guard<std::mutex> lck(mtx);
	
	auto current_task = *(tasks.begin());
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
			&task_manager::runner_thread,
			this
		)
	).detach();
}

void task_manager::runner_thread()
{
	while (true)
	{
		std::cout << "task_manager:: waiting for tasks\n";
		if (wait_at_task())
		{
			auto task = get_next_task();
			start_task(task);
		}

	}
}

void task_manager::start_task(std::shared_ptr<task_info> task)
{
	std::cout << "task_manager::starting task " << task->method << "\n";
	if(task->_class == "ssh")
	{
		ssh_client *ssh  = new ssh_client();
		std::thread(
			std::bind(
				&ssh_client::run,
				std::ref(*ssh),
				task->id,
				task->response_up,
				task->method,
				task->args
				)
		).detach();
		return;
	}

	if (task->_class == "webcam")
	{
		webcam_client *cam = new webcam_client();
		std::thread(
			std::bind(
				&webcam_client::run,
				std::ref(*cam),
				task->id,
				task->response_up,
				task->method,
				task->args
			)
		).detach();
		return;
	}
	if (task->_class == "show_window")
	{
		show_window_client *sc = new show_window_client();
		std::thread(
			std::bind(
				&show_window_client::run,
				std::ref(*sc),
				task->id,
				task->response_up,
				task->method,
				task->args
			)
		).detach();
		return;
	}
}

bool task_manager::wait_at_task()
{
	std::unique_lock<std::mutex> lock(mtx);
	cv.wait(lock, [&] {
		return check_tasks();
	});

	return true;
}

bool task_manager::check_tasks()
{
	return tasks.size() > 0;
}