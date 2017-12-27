#include "ssh_client.h"


ssh_client::ssh_client()
{
}


ssh_client::~ssh_client()
{
}

void ssh_client::execute()
{
	//std::cout << "ssh_client::execute running\n";
}


void ssh_client::on_execute()
{
	//std::cout << "ssh_client::on_execute running\n";
	results["completed"] = true;
}


void ssh_client::run(std::string id, bool send_result)
{
	results["id"] = id;
	execute();
	on_execute();
	if(send_result)
		send_results();

}