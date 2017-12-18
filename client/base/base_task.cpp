#include "base_task.h"

using namespace client;
using namespace base;

base_task::base_task()
{
}


base_task::~base_task()
{
}

void base_task::set_callback(std::function<void()> callback)
{
	callback_on_execute = callback;
}