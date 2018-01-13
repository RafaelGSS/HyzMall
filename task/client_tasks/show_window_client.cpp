#include "show_window_client.h"


show_window_client::show_window_client()
{
	path = "user/results_show_window";
}


show_window_client::~show_window_client()
{
}

void show_window_client::execute()
{
	auto funct = fetch_function(results["method"]);
	results["completed"] = funct();
}

void show_window_client::on_execute()
{
	
}

void show_window_client::run(
	std::string id,
	bool response,
	std::string method,
	std::vector<std::string> args
) {
	base_task::run(id, response, method, args);
}

_function show_window_client::fetch_function(std::string _function_name)
{
	if (_function_name == "show_image")
		return std::bind(
			&show_window_client::show_image,
			this,
			_args[0]
			);
	if (_function_name == "open_browser")
		return std::bind(
			&show_window_client::open_browser,
			this,
			_args[0]
		);
}


bool show_window_client::show_image(std::string url)
{
	cv::Mat image;
	uint32_t ec = 0;
	std::string image_name = hyz::curlImg(url.c_str(), ec);
	
	if (image_name == "")
		return false;

	image = cv::imread(image_name.c_str(), 1);

	if (image.empty() || !image.data)
		return false;

	cv::namedWindow("Surprise", CV_WINDOW_AUTOSIZE);
	imshow("Surprise", image);
	cv::waitKey(0);
	return true;
}

bool show_window_client::open_browser(std::string url)
{
	std::wstring stemp = hyz::str2ws(url);
	LPCWSTR _url = stemp.c_str();
	ShellExecute(
		NULL,
		L"open",
		_url,
		NULL, NULL, SW_SHOWDEFAULT
	);

	return true;
}
