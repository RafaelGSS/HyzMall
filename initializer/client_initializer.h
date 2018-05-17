#pragma once
#include <iostream>
#include <http\http_request.h>

namespace initializer {
	class client_initializer
	{
	private:
		bool check_one_instance();
	public:
		void run();
		client_initializer();
		static client_initializer* get();
	};
}