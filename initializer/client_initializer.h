#pragma once
#include <iostream>
#include <http\http_request.h>

namespace initializer {
	class client_initializer
	{
	private:
		void register_client();
	public:
		void run();
		client_initializer();
		~client_initializer();
		static client_initializer* get();
	};
}