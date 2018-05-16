#include "base_task.h"

using namespace client;
using namespace base;

base_task::base_task()
{
}

void base_task::run(
	std::string id,
	bool response,
	std::string method,
	std::vector<std::string> args
) {
	_args = args;
	initializeResults(id, method);
	execute();
	on_execute();
	if (response)
		send_results(results.dump(), path);
}


void base_task::send_results(std::string content, std::string _path, std::string hasFile)
{
	//std::cout << "added notification " << results.dump() << "\n";
	auto pPath = _path.size() > 0 ? _path : path;
	notification::get()->add_notification(pPath, content, hasFile);
}

void base_task::initializeResults(std::string _id, std::string method)
{
	results["id"] = _id;
	results["method"] = method;
	path_on_method = base_path + method;

}