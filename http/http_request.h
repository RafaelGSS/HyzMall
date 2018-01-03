#pragma once
#include "base\http_code.h"
#include "json\json.hpp"
//#include <bin\includes\pool_request.h>
#include <http\base\internal_request.h>
#include <http\base\internal_manager.h>

#define _DEBUG_ACTIVE 1

namespace http {
	using json_var = nlohmann::json;

	class http_request
	{
	public:
		http_request();
		static http_request* get();
		//~http_request();
		std::string post_request(std::string file, std::string content, uint32_t& ec, uint32_t max_tries = 10);
		//http_code get_request(std::string file, std::string content, uint32_t& ec, uint32_t max_tries = 10);
		json_var get_basic_body();
		//pool_request::request_ptr create_request(std::string sub_path);
		internal_ptr create_request(std::string sub_path);
	};
}



