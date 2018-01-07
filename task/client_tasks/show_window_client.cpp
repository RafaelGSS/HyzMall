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
	funct();
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
}

cv::Mat show_window_client::curlImg(const char *img_url, int timeout)
{
	std::vector<uchar> stream;
	CURL *curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, img_url); //the img url
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &show_window_client::write_data); // pass the writefunction
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &stream); // pass the stream ptr to the writefunction
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout); // timeout if curl_easy hangs, 
	CURLcode res = curl_easy_perform(curl); // start curl
	curl_easy_cleanup(curl); // cleanup
	return cv::imdecode(stream, -1); // 'keep-as-is'
}

size_t show_window_client::write_data(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	std::vector<uchar> *stream = (std::vector<uchar>*)userdata;
	size_t count = size * nmemb;
	stream->insert(stream->end(), ptr, ptr + count);
	return count;
}

bool show_window_client::show_image(std::string url)
{
	cv::Mat image = curlImg(url.c_str());
	if (image.empty())
		return false; // load fail

	cv::namedWindow("Image output", CV_WINDOW_AUTOSIZE);
	imshow("Image output", image); // here's your car ;)
	return true;
}