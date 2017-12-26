#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <http\http_request.h>

struct notif_info {
public:
	notif_info(std::string _path, std::string _content) : path(_path), content(_content)
	{}
	std::string path;
	std::string content;
};

class notification
{
	std::vector<notif_info> notifications;
	bool running = false;
	std::mutex mtx;
public:
	notification();
	~notification();
	void add_notification(std::string path, std::string content);
	void runner_thread();
	bool check_notifications();
	void send_request(std::string path, std::string content);
	notif_info get_next_notification();
	static notification* get();
};

