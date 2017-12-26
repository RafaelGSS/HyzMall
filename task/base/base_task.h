#pragma once
#include <functional>
#include <http\json\json.hpp>
#include <notifier/notification.h>

using json_var = nlohmann::json;

namespace client {
	namespace base {
		class base_task
		{
		public:
			base_task();
			~base_task();
			void send_results();
			virtual void execute() = 0;
			virtual void on_execute() = 0;
			virtual void run(std::string id, bool send) = 0;
		protected:
			json_var results;
		};
	}
}