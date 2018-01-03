#include "internal_request.h"

using namespace http;
using namespace base;

internal_request::internal_request()
{
	timeout = 600;

	validation = [](std::shared_ptr<std::string> res) -> bool {
		return true;
	};
}



internal_request::~internal_request()
{
}

std::string internal_request::get_server_name()
{
	return server_name;
}

void internal_request::set_server_name(const std::string& pool_name)
{
	this->server_name = pool_name;
}

std::string internal_request::get_sub_path()
{
	return sub_path;
}

void internal_request::set_sub_path(const std::string& sub_path)
{
	this->sub_path = sub_path;
}

std::string internal_request::get_data()
{
	return data;
}

void internal_request::set_data(const std::string& data)
{
	this->data = data;
}

uint32_t internal_request::get_timeout()
{
	return timeout;
}

void internal_request::set_timeout(uint32_t timeout)
{
	this->timeout = timeout;
}

void internal_request::add_arg(
	std::string arg_type,
	std::string arg_name,
	std::string arg_value
) {
	this->args[arg_type][arg_name] = arg_value;
}

std::function<bool(std::shared_ptr<std::string> res)> internal_request::get_cb_validation()
{
	return validation;
}

void internal_request::set_cb_validation(std::function<bool(std::shared_ptr<std::string> res)>& cb_validation)
{
	this->validation = cb_validation;
}

args_map internal_request::get_args()
{
	return this->args;
}