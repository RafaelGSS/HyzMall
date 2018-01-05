#include "internal_manager.h"

using namespace http;
using namespace base;

internal_manager::internal_manager()
{
}


internal_manager::~internal_manager()
{
}

internal_manager* internal_manager::get()
{
	static internal_manager* singleton = nullptr;
	if (!singleton)
		singleton = new internal_manager();

	return singleton;
}

internal_server_ptr internal_manager::get_server()
{
	return server;
}

std::shared_ptr<std::string> internal_manager::send(internal_ptr r, uint32_t& ec, bool hasFile, uint32_t max_tries)
{
	if (!max_tries)
	{
		return nullptr;
	}

	client_base client_curl;
	auto server = get_server();

	std::string subpath;
	if (r->get_sub_path() != "")
	{
		subpath = r->get_sub_path();
	}
	else
	{
		subpath = server->default_subpath;
	}
	std::string url = server->address + subpath;
	try
	{
		auto args = r->get_args();
		if (args.find("headers") != args.end())
		{
			for (auto it = args["headers"].begin(); it != args["headers"].end(); ++it)
			{
				client_curl.set_header(it->first, it->second);
			}
		}

		std::shared_ptr<std::string> res;
		if(hasFile)
			res = std::make_shared<std::string>(client_curl.upload(url, r->get_data()));
		else
			res = std::make_shared<std::string>(client_curl.post(url, r->get_data()));

		if (r->get_cb_validation()(res))
		{
			ec = 0;
			return res;
		}
		else
		{
			throw std::exception("Callback validation error", 1);
		}
	}
	catch (std::exception e)
	{
		ec = 500;
		return send(r, ec, --max_tries);
	}
}


void internal_manager::add_server_to_pool(
	std::string server_name,
	std::string hostname,
	std::string server_path
	)
{
	// TODO - mutex here
	this->server = (std::make_shared<internal_server>(server_name, hostname, server_path));
}

internal_server::internal_server(
	std::string server_name,
	std::string address,
	std::string default_subpath
)
{
	this->server_name = server_name;
	this->address = address;
	this->default_subpath = default_subpath;
}