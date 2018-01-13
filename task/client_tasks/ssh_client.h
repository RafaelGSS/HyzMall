#pragma once
#include <task\base\base_task.h>
#include <functional>
#include <vector>

class ssh_client : public client::base::base_task
{
public:
	ssh_client();
	~ssh_client();
	void execute();
	void on_execute();
	void run(
		std::string id,
		bool response,
		std::string method,
		std::vector<std::string> args
	);
	_function fetch_function(std::string);
	bool open_ssh();
};

