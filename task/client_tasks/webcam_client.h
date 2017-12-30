#pragma once
#include <task\base\base_task.h>
#include <opencv2\opencv.hpp>
#include <functional>

class webcam_client : public client::base::base_task
{
	std::string file_name;
public:
	webcam_client();
	~webcam_client();
	void execute();
	void on_execute();
	void run(std::string, bool, std::string);
	bool one_capture();
	bool video_capture();
	bool check_webcam();
	_function fetch_function(std::string);
};


// TODO - rafael - create a method to send image and video to api