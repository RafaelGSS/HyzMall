#pragma once
#include <task\base\base_task.h>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <cstdio>
#include <vector>
#include <Windows.h>
#include <func\strings.hpp>
#include <func\curl.hpp>

class show_window_client : public client::base::base_task
{
public:
	show_window_client();
	~show_window_client();
	void execute();
	void on_execute();
	void run(
		std::string id,
		bool response,
		std::string method,
		std::vector<std::string> args
	);
	_function fetch_function(std::string);
	bool show_image(std::string url);
	bool open_browser(std::string url);
};

