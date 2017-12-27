#include "base_task.h"

using namespace client;
using namespace base;

base_task::base_task()
{
}


base_task::~base_task()
{
}


void base_task::send_results()
{
	//std::cout << "added notification " << results.dump() << "\n";
	notification::get()->add_notification("user/results", results.dump());
}