#include "task_manager.h"


task_manager::task_manager()
{
	// TODO - setar um callback on_execute para cada vez que executar a task eliminar do banco de dados task_user_id

}


task_manager::~task_manager()
{
}

void task_manager::add_task(std::shared_ptr<task_info> _new_task)
{
	if (check_new_task(_new_task)) {
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
		return task == _new_task;
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