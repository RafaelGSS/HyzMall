#include <notifier\notification.h>
#include <stdio.h>


notification::notification()
{
	if (!running) {
		running = true;
		std::thread(
			runner_thread
		).detach();
	}
}


notification::~notification()
{
}

void notification::add_notification(std::string path, std::string content)
{
	notif_info new_notification(path, content);
	notifications.push_back(new_notification);
	std::cout << "new notification added path: " << path << " content: " << content << "\n";
}

notification* notification::get()
{
	static notification* singleton = nullptr;
	if (!singleton)
		singleton = new notification();
	return singleton;
}

void notification::runner_thread()
{
	while (true)
	{
		if (check_notifications())
		{
			auto _notification = get_next_notification();
			send_request(
				_notification.path,
				_notification.content
				);
		}
	}
}

bool notification::check_notifications()
{
	return notifications.size() > 0;
}

notif_info notification::get_next_notification()
{
	std::lock_guard<std::mutex> lck(mtx);

	auto current_notification = *(notifications.begin());
	notifications.erase(
		notifications.begin()
	);

	return current_notification;

}

void notification::send_request(
	std::string path, 
	std::string content
)
{
	uint32_t ec = 0;
	http::http_request::post_request(path, content, ec);
	if (ec)
		std::cout << "error " << ec << " send_request::notifier\n";

}