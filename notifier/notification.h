#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <cstdio>
#include <condition_variable>
#include <http\http_request.h>

struct notif_info {
public:
	notif_info(std::string _path, std::string _content, std::string _hasFile = std::string()) : path(_path), content(_content), hasFile(_hasFile)
	{}
	std::string path;
	std::string content;
	std::string hasFile;
};

class notification
{
	std::vector<notif_info> notifications;
	bool running = false;
	std::mutex mtx;
	std::condition_variable cv;
public:
	notification();
	void add_notification(std::string path, std::string content, std::string _hasFile = std::string());
	void runner_thread();
	bool check_notifications();
	void send_request(std::string path, std::string content);
	void send_request_file(std::string path, std::string conten, std::string file);
	bool wait_at_notification();
	notif_info get_next_notification();
	static notification* get();
};

