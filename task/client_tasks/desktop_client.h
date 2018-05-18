#pragma once
#include <task\base\base_task.h>

#include <cstdio>

#include <func\strings.hpp>
#include <func\image.hpp>

class desktop_client : public client::base::base_task
{
	std::string file_name;
public:
	desktop_client();
	void execute();
	void on_execute();
	void run(
		std::string id,
		bool response,
		std::string method,
		std::vector<std::string> args
	);
	_function fetch_function(std::string);
	bool desktop_capture();
};

