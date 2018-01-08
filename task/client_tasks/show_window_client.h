#pragma once
#include <task\base\base_task.h>
#include <opencv2\opencv.hpp>
#include <vector>

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
	cv::Mat curlImg(const char *img_url, int timeout = 10);
	//size_t write_data(char *ptr, size_t size, size_t nmemb, void *userdata);
};

