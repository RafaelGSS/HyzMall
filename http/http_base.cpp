#pragma once
#include "http_base.h"

pool_request::request_ptr http_base::create_request(std::string sub_path) {
	auto r = pool_request::create_request();
	r->set_pool_name("nettask");
	r->set_sub_path("/api/nettask/public/" + sub_path);
	r->add_arg("headers", "Content-Type", "application/json");
	return r;
}

std::string http_base::post_request(std::string file, std::string content, uint32_t& ec, uint32_t max_tries)
{
	auto req = create_request(file);
	req->set_data(content);

	auto res = pool_request::get_pool_manager()->send(req, ec, max_tries);

	if (res != nullptr) {
		return *res;
	}
	return "";
}

http_base::http_base() {
	static pool_request::pool_manager* pm;
	if (!pm) {
		pm = pool_request::get_pool_manager();
		std::string pool_name = "nettask";

		//std::string pool_path = "/api/nettask";

		std::string pool_path = "/api/nettask/public";
		std::string hostName = "http://167.114.119.96";
		pm->add_server_to_pool(pool_name, hostName, pool_path);

		/*for (uint32_t i = 1; i < 11; i++) {
		pm->add_server_to_pool(pool_name, "http://aux" + std::to_string(i) + ".scutumnet.com", pool_path);
		}*/
	}
}

json_var http_base::get_basic_body()
{
	json_var basic;
	basic["request"] = "true";
	return basic;

}