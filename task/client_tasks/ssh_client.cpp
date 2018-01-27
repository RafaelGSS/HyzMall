#include "ssh_client.h"


ssh_client::ssh_client()
{
	path = "user/results_ssh";
}


ssh_client::~ssh_client()
{
}

void ssh_client::execute()
{
	auto funct = fetch_function(results["method"]);
	funct();
}

void ssh_client::run(
	std::string id,
	bool response,
	std::string method,
	std::vector<std::string> args
)
{
	base_task::run(
		id,
		response,
		method,
		args
	);
}

void ssh_client::on_execute()
{
	results["completed"] = true;
}


_function ssh_client::fetch_function(std::string method)
{
	if (method == "open")
		return std::bind(&ssh_client::open_ssh, this);

	return _function();
}

bool ssh_client::open_ssh()
{
	return true;
}