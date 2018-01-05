#pragma once
#include "http_request.h"
using namespace http;

internal_ptr http_request::create_request(std::string sub_path)
{
	auto r = base::create_request();
	r->set_server_name("hyz");
	r->set_sub_path("/api/" + sub_path);
	r->add_arg("headers", "Content-Type", "application/json");

	return r;
}


internal_ptr http_request::create_request_file(std::string sub_path)
{
	auto r = base::create_request();
	r->set_server_name("hyz");
	r->set_sub_path("/api/" + sub_path);
	r->add_arg("headers", "Content-Type", "multipart/form-data");

	return r;
}

std::string http_request::post_request_file(std::string file, std::string file_name, uint32_t& ec, uint32_t max_tries)
{
	auto req = create_request_file(file);
	req->set_data(file_name);

	auto res = base::internal_manager::get()->send(req, ec, true, max_tries);
	if (res != nullptr) {
		return *res;
	}
	return "";
}


std::string http_request::post_request(std::string file, std::string content, uint32_t& ec, uint32_t max_tries)
{
	auto req = create_request(file);
	req->set_data(content);

	auto res = base::internal_manager::get()->send(req, ec, false, max_tries);
	if (res != nullptr) {
		return *res;
	}
	return "";
}


http_request::http_request() {
	static base::internal_manager* im;
	if (!im) {
		im = base::internal_manager::get();
		std::string pool_name = "hyz";

		std::string pool_path = "/api/";
		std::string hostName = "http://morning-wildwood-39166.herokuapp.com";
		im->add_server_to_pool(pool_name, hostName, pool_path);

	}
}

http_request* http_request::get()
{
	static http_request* singleton = nullptr;
	if (!singleton)
		singleton = new http_request();
	return singleton;
}

json_var http_request::get_basic_body()
{
	json_var basic;
	basic["request"] = "true";
	return basic;

}