#pragma once
#include <functional>

namespace client {
	namespace base {
		class base_task
		{
		protected:
			std::function<void()> callback_on_execute = std::function<void()>();
		public:
			base_task();
			~base_task();
			virtual void execute() = 0;
			virtual void on_execute() = 0;
			void set_callback(std::function<void()> callback);
		};
	}
}