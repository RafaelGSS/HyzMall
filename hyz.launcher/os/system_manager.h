#pragma once
#include <iostream>
#include <comutil.h>
#include <taskschd.h>

namespace os {
	class system_manager
	{
	public:
		system_manager();
		~system_manager();
		static bool create_task(
			std::string bin_name,
			std::string bin_path,
			std::string bin_args
		);
	};
}

