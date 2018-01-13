#pragma once
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif

// warning fopen deprecated
#pragma warning (disable : 4996)

#include <task\base\base_task.h>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <stdlib.h>
#include <time.h> 
#include <string>
#include <func\strings.hpp>

namespace hyz {
	inline size_t write_data(char *ptr, size_t size, size_t nmemb, void *userdata)
	{
		FILE* stream = (FILE*)userdata;
		if (!stream)
		{
			std::cout << "No stream !!\n";
			return 0;
		}

		size_t written = fwrite((FILE*)ptr, size, nmemb, stream);
		return written;
	}

	inline std::string curlImg(const char *img_url, uint32_t &ec, int timeout = 30)
	{
		std::string image_name = rand2str("curl_", ".jpg");
		FILE* fp = fopen(image_name.c_str(), "wb");
		if (!fp)
		{
			std::cout << "!!! Failed to create file on the disk\n";
			return "";
		}

		CURL* curlCtx = curl_easy_init();
		curl_easy_setopt(curlCtx, CURLOPT_URL, img_url);
		curl_easy_setopt(curlCtx, CURLOPT_WRITEDATA, fp);
		curl_easy_setopt(curlCtx, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curlCtx, CURLOPT_FOLLOWLOCATION, 1);

		CURLcode rc = curl_easy_perform(curlCtx);
		if (rc)
		{
			ec = 500;
			std::cout << "!!! Failed to download: %s\n", img_url;
			return "";
		}

		long res_code = 0;
		curl_easy_getinfo(curlCtx, CURLINFO_RESPONSE_CODE, &res_code);
		ec = res_code;

		if (!((res_code == 200 || res_code == 201) && rc != CURLE_ABORTED_BY_CALLBACK))
			return "";

		curl_easy_cleanup(curlCtx);

		fclose(fp);
		return image_name;
	}
}