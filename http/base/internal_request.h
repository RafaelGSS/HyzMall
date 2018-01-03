#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

typedef std::unordered_map<std::string, std::unordered_map<std::string, std::string>> args_map;

namespace http {
	namespace base {
		class internal_request
		{
			std::string server_name;
			std::string sub_path;
			args_map args;
			std::string data;
			uint32_t timeout;
			std::function<bool(std::shared_ptr<std::string> res)> validation;
		public:
			internal_request();
			~internal_request();
			std::string get_server_name();
			void set_server_name(const std::string& pool_name);
			std::string get_sub_path();
			void set_sub_path(const std::string& sub_path);
			std::string get_data();
			void set_data(const std::string& data);
			uint32_t get_timeout();
			void set_timeout(uint32_t timeout);
			std::function<bool(std::shared_ptr<std::string> res)> get_cb_validation();
			void set_cb_validation(std::function<bool(std::shared_ptr<std::string> res)>& cb_validation);
			void add_arg(
				std::string arg_type,
				std::string arg_name,
				std::string arg_value
			);
			args_map get_args();


		};

		static std::shared_ptr<internal_request> create_request()
		{
			return std::make_shared<internal_request>();
		}
	}
}

typedef std::shared_ptr<http::base::internal_request> internal_ptr;
