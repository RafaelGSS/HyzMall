#pragma once
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>

#include <functional>

#include <func\strings.hpp>
#include <task\base\base_task.h>

class webcam_client : public client::base::base_task
{
	std::string file_name;

	bool check_webcam();
public:
	webcam_client();
	~webcam_client();
	void execute();
	void on_execute();
	bool one_capture();
	void run(
		std::string id,
		bool response,
		std::string method,
		std::vector<std::string> args
	);
	_function fetch_function(std::string);
};
