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
	auto funct = fetch_function(results["args_type"]);
	funct();
}


void ssh_client::on_execute()
{
	results["completed"] = true;
}


void ssh_client::run(std::string id, bool send_result, std::string args)
{
	initializeResults(id, args);
	execute();
	on_execute();
	if(send_result)
		send_results(results.dump());

}

_function ssh_client::fetch_function(std::string _function)
{
	if (_function == "open")
		return std::bind(&ssh_client::open_ssh, this);
}

bool ssh_client::open_ssh()
{
	return true;
}