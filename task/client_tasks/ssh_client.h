#pragma once
#include <task\base\base_task.h>

class ssh_client : public client::base::base_task
{
public:
	ssh_client();
	~ssh_client();
	void execute();
	void on_execute();
};

