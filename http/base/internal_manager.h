#pragma once
#include <memory>
#include <string>
#include <http\base\internal_request.h>
#include <http\base\base_request.h>

namespace http {
	namespace base {

		struct internal_server {
			internal_server(
				std::string server_name,
				std::string address,
				std::string default_subpath = ""
			);
			std::string server_name;
			std::string address;
			std::string default_subpath;
		};

		typedef std::shared_ptr<internal_server> internal_server_ptr;

		class internal_manager
		{
			internal_server_ptr server;
		public:
			internal_manager();
			~internal_manager();
			static internal_manager* get();
			internal_server_ptr get_server();
			std::shared_ptr<std::string> send(internal_ptr r, uint32_t& ec, uint32_t max_tris = 10);
			void add_server_to_pool(std::string server_name, std::string hostname, std::string server_path);
		};
	}
}

