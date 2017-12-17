#pragma once
#include "../stdafx.h"
#include "http_code.h"
#include "json\json.hpp"
#include <bin\includes\pool_request.h>

#define _DEBUG_ACTIVE 1

using json_var = nlohmann::json;

class http_base
{
public:
	http_base();
	//~http_base();
protected:
	std::string post_request(std::string file, std::string content, uint32_t& ec, uint32_t max_tries = 10);
	//http_code get_request(std::string file, std::string content, uint32_t& ec, uint32_t max_tries = 10);
	json_var get_basic_body();
	pool_request::request_ptr create_request(std::string sub_path);
};



