#pragma once
#include "http_code.h"
#include "json\json.hpp"

using json_var = nlohmann::json;

class http_base
{
public:
	http_base();
	~http_base();
protected:
	http_code post_request();
	http_code get_request();
	json_var create_header();
};

