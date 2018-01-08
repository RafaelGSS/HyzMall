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

size_t write_data(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	FILE* stream = (FILE*)userdata;
	if (!stream)
	{
		printf("!!! No stream\n");
		return 0;
	}

	size_t written = fwrite((FILE*)ptr, size, nmemb, stream);
	return written;
}

cv::Mat show_window_client::curlImg(const char *img_url, int timeout)
{
	FILE* fp = fopen("out.jpg", "wb");
	if (!fp)
	{
		printf("!!! Failed to create file on the disk\n");
		return cv::Mat();
	}

	CURL* curlCtx = curl_easy_init();
	curl_easy_setopt(curlCtx, CURLOPT_URL, img_url);
	curl_easy_setopt(curlCtx, CURLOPT_WRITEDATA, fp);
	curl_easy_setopt(curlCtx, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curlCtx, CURLOPT_FOLLOWLOCATION, 1);

	CURLcode rc = curl_easy_perform(curlCtx);
	if (rc)
	{
		printf("!!! Failed to download: %s\n", img_url);
		return cv::Mat();
	}

	long res_code = 0;
	curl_easy_getinfo(curlCtx, CURLINFO_RESPONSE_CODE, &res_code);
	if (!((res_code == 200 || res_code == 201) && rc != CURLE_ABORTED_BY_CALLBACK))
	{
		printf("!!! Response code: %d\n", res_code);
		return cv::Mat();
	}

	curl_easy_cleanup(curlCtx);

	fclose(fp);

	return cv::imread("out.jpg", 1);
}

bool show_window_client::show_image(std::string url)
{
	// ERROR - show only 1 image
	cv::Mat image;
	image = curlImg(url.c_str());
	if (image.empty() || !image.data)
		return false; // load fail

	cv::namedWindow("Surprise", CV_WINDOW_AUTOSIZE);
	imshow("Surprise", image); // here's your car ;)
	cv::waitKey(0);
	return true;
}