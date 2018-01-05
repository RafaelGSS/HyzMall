#include "base_task.h"

using namespace client;
using namespace base;

base_task::base_task()
{
}


base_task::~base_task()
{
}


void base_task::send_results(std::string content, bool hasFile)
{
	//std::cout << "added notification " << results.dump() << "\n";
	notification::get()->add_notification(path, content, hasFile);
}

void base_task::initializeResults(std::string _id, std::string _args)
{
	results["id"] = _id;
	results["args_type"] = _args;
}