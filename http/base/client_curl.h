#pragma once
#include <vector>
#include <map>
#include <stdint.h>

#include <sstream>
#include <iostream>
#include <ostream>
#include <iterator>
#include <functional>

#include <boost/thread.hpp>
#include <thread>
#include <chrono>

#ifndef curl_lib
#define curl_lib CURL
#endif

namespace http {
	namespace base {
		class client_curl;

		struct curl_progress {
			double last_runtime;
			curl_lib *curl;
		};

		class curl_progress_notifier {
			wpp::thread::sync::mutex _mtx;
			std::map<curl_lib*, void*> owners;

			std::function<
				int(void *p,
					double dltotal, double dlnow,
					double ultotal, double ulnow)
			>
				_callback_curl_on_progress;

		public:

			void set_static_on_progress(
				std::function<
				int(void *p,
					double dltotal, double dlnow,
					double ultotal, double ulnow)
				>
				___callback
			) {
				_callback_curl_on_progress = ___callback;
			}

			std::function<
				int(void *p,
					double dltotal, double dlnow,
					double ultotal, double ulnow)
			> get_static_on_progress() {
				return _callback_curl_on_progress;
			}
			void set_owner(void * _curl, void* owner) {
				wpp::thread::sync::lock_guard<wpp::thread::sync::mutex> _lock(_mtx);
				owners[_curl] = owner;
			}

			void* get_owner(void * _curl) {
				wpp::thread::sync::lock_guard<wpp::thread::sync::mutex> _lock(_mtx);
				auto _it = owners.find(_curl);
				if (_it == owners.end())
					return nullptr;
				return _it->second;
			}

			void release(void* _curl) {
				wpp::thread::sync::lock_guard<wpp::thread::sync::mutex> _lock(_mtx);
				auto _it = owners.find(_curl);
				if (_it == owners.end())
					owners.erase(_it);
			}

			int on_progress(
				void *p,
				double dltotal, double dlnow,
				double ultotal, double ulnow
			) {
				if (!p)
					return 0;

				if (_callback_curl_on_progress) {
					return _callback_curl_on_progress(
						p, dltotal, dlnow, ultotal, ulnow
					);
				}
				return 0;
			}

			static curl_progress_notifier* get() {
				static curl_progress_notifier* mcurl_progress_notifier = nullptr;
				if (!mcurl_progress_notifier) {
					static wpp::thread::sync::mutex _mtx_access;
					_mtx_access.lock();
					if (!mcurl_progress_notifier) {
						mcurl_progress_notifier = new curl_progress_notifier();
					}
					_mtx_access.unlock();
				}

				return mcurl_progress_notifier;
			}
		};

		static int curl_on_progress(void *p,
			double dltotal, double dlnow,
			double ultotal, double ulnow
		)
		{
			if (!p)
				return 0;

			curl_progress *myp = (curl_progress *)p;
			return curl_progress_notifier::get()->on_progress(
				myp->curl, dltotal, dlnow, ultotal, ulnow
			);
		}

		class client_curl : public wpp::net::http::client_base {
			//friend curl_progress_notifier;

			int on_api_progress(
				void* /*p*/,
				double dltotal, double dlnow,
				double /*ultotal*/, double /*ulnow*/
			) {
				this->async_info.received = (size_t)dlnow;
				this->async_info.total = (size_t)dltotal;
				if (this->_event_handler_progress) {
					return this->_event_handler_progress(this, &async_info);
				}
				//this->_event_handler_progress
				return CURLE_OK;
			}
		private:
			class download_filter {
			public:
				typedef std::function<bool(size_t, size_t)> on_progress;
			private:
				std::string content;
				std::ofstream* _out_file;
				on_progress _on_progress;

			public:
				download_filter(std::ofstream* _file) {
					_out_file = _file;
				}

				download_filter() : _out_file(nullptr) {
				}

				void set_on_progress_event(std::function<bool(size_t, size_t)> _evt) {
					_on_progress = _evt;
				}

				static size_t handle(char * data, size_t size, size_t nmemb, void * p) {
					return static_cast<download_filter*>(p)->handle_impl(data, size, nmemb);
				}

				size_t handle_impl(char * data, size_t size, size_t nmemb) {
					if (_out_file) {
						_out_file->write(data, size * nmemb);
					}
					else {
						content.append(data, size * nmemb);
					}
					return size * nmemb;
				}

				std::string get_content() {
					return content;
				}
			};


			void add_headers(struct curl_slist** headers_ptr)
			{
				for (auto& header : headers) {
					*headers_ptr = curl_slist_append(*headers_ptr, (header.first + ": " + header.second).data());
				}

				if (!has_header("Cache-Control"))
					*headers_ptr = curl_slist_append(*headers_ptr, "Cache-Control: private, no-cache, no-store, must-revalidate, max-age=0");

				if (!has_header("Expect"))
					*headers_ptr = curl_slist_append(*headers_ptr, "Expect:  ");

				if (!has_header("Pragma"))
					*headers_ptr = curl_slist_append(*headers_ptr, "Pragma: no-cache");

			}

			long curl_post_file(
				std::string url,
				std::string& stream_out,
				std::string content = std::string(),
				std::map<std::string, std::string> args = std::map<std::string, std::string>(),
				std::string file_name = std::string(),
				uint32_t timeout = (uint32_t)-1
			) {
				lock();
				if (file_name.size()) {
					if (!set_file(file_name)) {
						unlock();
						return false;
					}
				}

				curl_lib *curl;
				CURLcode res = CURLE_OK;
				curl = curl_easy_init();
				if (!curl) {
					unlock();
					return 0;
				}


				download_filter filter(out_file);

				curl_easy_setopt(curl, CURLOPT_URL, &url[0]);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &filter);
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &download_filter::handle);

				/*
				if content is empty then look for args map
				*/
				if (!content.size()) {
					if (args.size()) {

						std::vector<std::string> strings;
						for (auto& kv : args) {
							strings.push_back(kv.first + "=" + kv.second);
						}

						std::ostringstream imploded;
						std::copy(strings.begin(), strings.end(),
							std::ostream_iterator<std::string>(imploded, "&"));

						content = imploded.str();
					}
				}

				// Internal curl_lib progressmeter must be disabled if we provide our own callback
				curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);

				curl_progress_notifier::get()->set_owner(curl, this);

				curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, curl_on_progress);

				curl_progress prog;
				prog.last_runtime = 0;
				prog.curl = curl;
				/* pass the struct pointer into the progress function */
				curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &prog);

				//if (content.size()) {
				curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, content.size());
				curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content.data());
				//}

				url +=
					(url.find("?") == std::string::npos ? "?" : "&") + std::string("request_tst=")
					+ std::to_string(std::chrono::milliseconds(time(0)).count());

				struct curl_slist *curl_headers = 0; /* init to 0 is important */
				add_headers(&curl_headers);

				if (timeout && timeout != -1) {
					curl_easy_setopt(curl, CURLOPT_TIMEOUT, (long)(timeout / 1000));
				}

				curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_headers);

				res = curl_easy_perform(curl);
				long http_code = 0;
				curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

				curl_slist_free_all(curl_headers); /* free the header list */
				curl_easy_cleanup(curl);
				stream_out = filter.get_content();

				if (res != CURLE_OK) {
					release_file();
					curl_progress_notifier::get()->release(curl);
					unlock();
					//std::cout << "\n Curl error code " << res;
					return -1;
				}


				release_file();
				curl_progress_notifier::get()->release(curl);
				unlock();
				return http_code;
			}

			virtual uint32_t download_string(
				char* _url,
				std::string& out,
				std::string file_name,
				uint32_t timeout = (uint32_t)-1
			) {
				lock();
				if (file_name.size()) {
					if (!set_file(file_name)) {
						unlock();
						return false;
					}
				}

				std::string url = _url;

				url +=
					(url.find("?") == std::string::npos ? "?" : "&") + std::string("request_tst=")
					+ std::to_string(std::chrono::milliseconds(time(0)).count());


				curl_lib *curl;
				CURLcode res = CURLE_OK;
				curl = curl_easy_init();

				if (!curl) {
					unlock();
					return 0;
				}

				// Internal curl_lib progressmeter must be disabled if we provide our own callback
				curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);

				curl_progress_notifier::get()->set_owner(curl, this);
				curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, curl_on_progress);


				download_filter filter(out_file);
				curl_progress prog;
				prog.last_runtime = 0;
				prog.curl = curl;
				/* pass the struct pointer into the progress function */
				curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &prog);

				if (timeout && timeout != -1) {
					curl_easy_setopt(curl, CURLOPT_TIMEOUT, (long)(timeout / 1000));
				}
				curl_easy_setopt(curl, CURLOPT_URL, url.data());
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &filter);
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &download_filter::handle);

				struct curl_slist *curl_headers = 0; /* init to 0 is important */
				add_headers(&curl_headers);


				curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_headers);

				res = curl_easy_perform(curl);

				long http_code = 0;
				curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

				curl_slist_free_all(curl_headers); /* free the header list */
				curl_easy_cleanup(curl);
				out = filter.get_content();



				release_file();
				curl_progress_notifier::get()->release(curl);
				unlock();

				if (res != CURLE_OK) {
					//std::cout << "\n Curl error code " << res;
					return (uint32_t)-1;
				}


				return http_code;
			}

			std::ofstream* out_file;

			bool set_file(std::string file_name) {
				release_file();
				out_file = new std::ofstream(file_name, std::ios::ios_base::binary);
				if (!out_file->is_open())
					return false;
				return true;
			}

			void release_file() {
				if (out_file) {
					if (out_file->is_open()) {
						out_file->close();
						delete out_file;
					}
					out_file = nullptr;
				}
			}

		protected:
			virtual std::string impl_post(
				std::string url,
				std::string content,
				http_response_code& result_code,
				uint32_t timeout = -1
			) {
				std::string retval;
				result_code =
					(http_response_code)
					curl_post_file(
						url,
						retval,
						content,
						std::map < std::string, std::string >(),
						std::string(),
						timeout
					);
				return retval;
			}

			virtual std::string impl_post(
				std::string url,
				std::map<std::string, std::string> args,
				http_response_code& result_code,
				uint32_t timeout = -1
			) {
				return impl_post(url, args_to_content(args), result_code, timeout);
			}

			virtual std::string impl_get(
				std::string _url,
				http_response_code& result_code,
				uint32_t timeout = -1
			) {
				std::string out;
				result_code = (http_response_code)download_string((char*)_url.data(), out, "", timeout);
				return out;
			}

			virtual bool impl_download_file(
				std::string _url,
				std::string file_name,
				std::string args = std::string(),
				bool post = false,
				bool async = false,
				event_handler_progress __event_handler_progress = event_handler_progress(),
				event_handler_complete __event_handler_complete = event_handler_complete(),
				event_handler_fail __event_handler_fail = event_handler_fail(),
				uint32_t timeout = -1
			) {
				if (!file_name.size())
					return false;

				this->_event_handler_complete = __event_handler_complete;
				this->_event_handler_fail = __event_handler_fail;
				this->_event_handler_progress = __event_handler_progress;

				if (!async) {
					std::string result_placeholder;
					http_response_code res;

					if (post) {
						res = (http_response_code)download_string(
							(char*)_url.data(), result_placeholder, file_name, timeout);
					}
					else {
						res = (http_response_code)curl_post_file(
							_url, result_placeholder, args, std::map<std::string, std::string>(),
							file_name,
							timeout
						);
					}

					if (res == http_response_code::http_response_ok) {
						if (this->_event_handler_complete)
							this->_event_handler_complete(this);
						return true;
					}
					else {
						if (this->_event_handler_fail)
							this->_event_handler_fail(this);
						return false;
					}
				}
				else {
					bool _thread_safe_start = false;
					boost::thread(
						[&]() {
						http_response_code res;
						std::string __url = _url;
						std::string _file_name = file_name;
						std::string _args = args;
						bool _post = post;
						//bool _async = async;
						_thread_safe_start = true;
						std::string result_placeholder;
						if (_post) {
							res = (http_response_code)
								curl_post_file(__url, result_placeholder, args,
									std::map<std::string, std::string>(), _file_name,
									timeout
								);
						}
						else {
							res = (http_response_code)download_string(
								(char*)__url.data(), result_placeholder, _file_name, timeout);
						}

						if (res == http_response_code::http_response_ok) {
							if (this->_event_handler_complete)
								this->_event_handler_complete(this);
						}
						else {
							if (this->_event_handler_fail)
								this->_event_handler_fail(this);
						}
					}
					).detach();

					//todo implement event instead sleep loop
					while (!_thread_safe_start) {
						//todo implement event avoid.
						//sleep is not the best method..
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}
				}

				return true;
			}

			static int static_on_progress(
				void *p,
				double dltotal,
				double dlnow,
				double ultotal,
				double ulnow
			) {
				auto _owner = curl_progress_notifier::get()->get_owner(p);
				if (_owner) {
					return ((client_curl*)_owner)->
						on_api_progress(p, dltotal, dlnow, ultotal, ulnow);
				}
				return CURLE_OK;
			}

			static void install_on_progress() {
				static bool installed = false;
				static boost::mutex mtx_install;
				//already installed
				if (installed)
					return;

				boost::lock_guard < boost::mutex > _guard(mtx_install);
				if (installed)
					return;

				auto ___callback =
					std::bind(
						static_on_progress,
						std::placeholders::_1,
						std::placeholders::_2,
						std::placeholders::_3,
						std::placeholders::_4,
						std::placeholders::_5
					);

				curl_progress_notifier::get()->set_static_on_progress(
					___callback
				);

				installed = true;
			}

		public:
			static client_base* get() {
				static client_base* mclient_base = nullptr;
				if (!mclient_base)
					mclient_base = (client_base*)new client_curl();
				return mclient_base;
			}

			client_curl() {
				out_file = nullptr;
				//todo improve this.
				install_on_progress();
			}
		};
	}	  // namespace http
}


#endif