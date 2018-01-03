#pragma once
#include <vector>
#include <map>
#include <stdint.h>

#include <sstream>
#include <algorithm>
#include <iostream>
#include <ostream>
#include <iterator>
#include <functional>

#include <thread>
#include <chrono>
#include <fstream>

#include <libs/curl/include/curl/curl.h>
#include <libs/curl/include/curl/easy.h>
#include <http\base\http_code.h>

#include <mutex>
#pragma comment(lib, "libs/curl/libcurl.lib")
#define curl_lib CURL

namespace http {
#pragma pack(push, 1)
	class http_async_download_info {
	public:
		http_async_download_info() {
			received = (size_t)0;
			total = (size_t)0;
		}
		size_t received, total;
	};
#pragma pack(pop)
	class http_exception : public std::exception {
		std::string error_str;

		void make_str_error() {
			if (!error_str.size())
				error_str = "server return no ok code (" + std::to_string(__code) + ")";
		}
	public:

		virtual ~http_exception() throw() {

		}

		http_exception(http::http_code _code) {
			__code = _code;
			make_str_error();
		}

		virtual char const* what() const throw() {
			return error_str.data();
		}

		virtual http::http_code code() {
			return __code;
		}

	protected:
		http::http_code __code;
	};

	class client_base;

	struct curl_progress {
		double last_runtime;
		curl_lib *curl;
	};

	class curl_progress_notifier {
		std::recursive_mutex _mtx;
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
			std::lock_guard<std::recursive_mutex> _lock(_mtx);
			owners[_curl] = owner;
		}

		void* get_owner(void * _curl) {
			std::lock_guard<std::recursive_mutex> _lock(_mtx);
			auto _it = owners.find(_curl);
			if (_it == owners.end())
				return nullptr;
			return _it->second;
		}

		void release(void* _curl) {
			std::lock_guard<std::recursive_mutex> _lock(_mtx);
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
				static std::recursive_mutex _mtx_access;
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

	class client_base {
	public:
		bool _is_busy;
		bool _cancel;

		typedef std::function<bool(client_base*, http_async_download_info*)> event_handler_progress;
		typedef std::function<void(client_base*)> event_handler_complete;
		typedef std::function<void(client_base*)> event_handler_fail;

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

		//long curl_post_file(
		//	std::string url,
		//	std::string file_name = std::string(),
		//	std::string content = std::string(),
		//	std::map<std::string, std::string> args = std::map<std::string, std::string>(),
		//	uint32_t timeout = (uint32_t)-1
		//) {
		//	curl_lib *curl;
		//	CURLcode res = CURLE_OK;

		//	curl = curl_easy_init();

		//	if (!curl)
		//		return 0;

		//	curl_mime *form = curl_mime_init(curl);
		//	curl_mimepart *field = curl_mime_addpart(form);

		//	curl_mime_name(field, "sendfile");
		//	curl_mime_filedata(field, file_name);

		//	field = curl_mime_addpart(form);
		//	curl_mime_name(field, "filename");
		//	curl_mime_data(field, file_name, CURL_ZERO_TERMINATED);

		//	field = curl_mime_addpart(form);
		//	curl_mime_name(field, "submit");
		//	curl_mime_data(field, "send", CURL_ZERO_TERMINATED);

		//	struct curl_slist *curl_headers = 0; /* init to 0 is important */
		//	add_headers(&curl_headers);

		//	if (!content.size()) {
		//		if (args.size()) {

		//			std::vector<std::string> strings;
		//			for (auto& kv : args) {
		//				strings.push_back(kv.first + "=" + kv.second);
		//			}

		//			std::ostringstream imploded;
		//			std::copy(strings.begin(), strings.end(),
		//				std::ostream_iterator<std::string>(imploded, "&"));

		//			content = imploded.str();
		//		}
		//	}

		//	if (timeout && timeout != -1)
		//		curl_easy_setopt(curl, CURLOPT_TIMEOUT, (long)(timeout / 1000));

		//	curl_easy_setopt(curl, CURLOPT_URL, &url[0]);
		//	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_headers);
		//	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, content.size());
		//	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content.data());

		//	long http_code = 0;
		//	res = curl_easy_perform(curl);

		//	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

		//	curl_slist_free_all(curl_headers);
		//	curl_mime_free(form);
		//	curl_easy_cleanup(curl);

		//	if (res != CURLE_OK)
		//		return -1;

		//	return http_code;
		//}

		std::recursive_mutex* mtx_access;
		http_async_download_info async_info;

		void reset_async_info() {
			async_info = http_async_download_info();
		}

		void increase_receive(size_t _in) {
			async_info.received += _in;
			call_progress_handler();
		}

		void set_total_size(size_t _in) {
			async_info.total = _in;
			async_info.received = 0;
			call_progress_handler();
		}

		void release_handlers() {
			_event_handler_progress = event_handler_progress();
			_event_handler_complete = event_handler_complete();
			_event_handler_fail = event_handler_fail();
		}

		event_handler_progress _event_handler_progress;
		event_handler_complete _event_handler_complete;
		event_handler_fail _event_handler_fail;
		std::function<void()> _on_cancel_request;

		void call_complete_handler() {
			if (_event_handler_complete)
				_event_handler_complete(this);
			release_handlers();
		}

		void call_fail_handler() {
			if (_event_handler_fail)
				_event_handler_fail(this);
			release_handlers();
		}

		void call_progress_handler() {
			if (_event_handler_progress) {
				if (!_event_handler_progress(this, &async_info)) {
					release_handlers();
				}
			}
		}

		client_base() {
			_is_busy = false;
			_cancel = false;
			mtx_access = new std::recursive_mutex();
		}

		~client_base() {
			delete mtx_access;
		}

		void lock() {
			mtx_access->lock();
		}

		void unlock() {
			mtx_access->unlock();
		}

		std::string join(
			std::vector<std::string>& str_vector,
			std::string glue,
			bool keep_empty)
		{
			std::stringstream ss;

			if (str_vector.size() == 1) {
				return str_vector[0];
			}

			for (size_t i = 0; i < str_vector.size(); ++i)
			{
				if (keep_empty || i != 0)
					ss << glue;
				ss << str_vector[i];
			}
			return ss.str();
		}

		std::string merge_args_url(
			std::string url,
			std::map<std::string, std::string> _args
		) {
			std::vector<std::string> args_vector;
			for (auto& arg_pair : _args) {
				args_vector.push_back(arg_pair.first + "=" + arg_pair.second);
			}

			return merge_args_url(url, join(args_vector, "&", false));
		}

		std::string merge_args_url(
			std::string url,
			std::string _args_content
		) {
			if (url.find("?") == std::string::npos && _args_content.size())
				url += "?" + _args_content;
			else if (_args_content.size())
				url += "&" + _args_content;
			return url;
		}

		std::ofstream* out_file;

		bool set_file(
			std::string file_name
		) {
			release_file();
			out_file = new std::ofstream(file_name, std::ios::ios_base::binary);

			if (!out_file->is_open())
				return false;
			return true;
		}

		void release_file(
		) {
			if (out_file) {
				if (out_file->is_open()) {
					out_file->close();
					delete out_file;
				}
				out_file = nullptr;
			}
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

		virtual std::string impl_post(
			std::string url,
			std::string content,
			http::http_code& result_code,
			uint32_t timeout = (uint32_t)-1
		) {
			std::string retval;
			result_code =
				(http::http_code)
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
			http::http_code& result_code,
			uint32_t timeout = (uint32_t)-1
		) {
			return impl_post(url, args_to_content(args), result_code, timeout);
		}

		virtual std::string impl_get(
			std::string _url,
			http::http_code& result_code,
			uint32_t timeout = (uint32_t)-1
		) {
			std::string out;
			result_code = (http::http_code)download_string((char*)_url.data(), out, "", timeout);
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
			uint32_t timeout = (uint32_t)-1
		) {
			if (!file_name.size())
				return false;

			this->_event_handler_complete = __event_handler_complete;
			this->_event_handler_fail = __event_handler_fail;
			this->_event_handler_progress = __event_handler_progress;

			if (!async) {
				std::string result_placeholder;
				http::http_code res;

				if (post) {
					res = (http::http_code)download_string(
						(char*)_url.data(), result_placeholder, file_name, timeout);
				}
				else {
					res = (http::http_code)curl_post_file(
						_url, result_placeholder, args, std::map<std::string, std::string>(),
						file_name,
						timeout
					);
				}

				if (res == http::http_code::http_response_ok) {
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
				std::thread(
					[&]() {
					http::http_code res;
					std::string __url = _url;
					std::string _file_name = file_name;
					std::string _args = args;
					bool _post = post;
					//bool _async = async;
					_thread_safe_start = true;
					std::string result_placeholder;
					if (_post) {
						res = (http::http_code)
							curl_post_file(__url, result_placeholder, args,
								std::map<std::string, std::string>(), _file_name,
								timeout
							);
					}
					else {
						res = (http::http_code)download_string(
							(char*)__url.data(), result_placeholder, _file_name, timeout);
					}

					if (res == http::http_code::http_response_ok) {
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

		virtual std::string hook_post(
			std::string url,
			std::string content,
			http::http_code& result_code,
			uint32_t timeout = (uint32_t)-1
		) {
			return impl_post(url, content, result_code, timeout);
		}

		virtual std::string hook_post(
			std::string url,
			std::map<std::string, std::string> args,
			http::http_code& result_code,
			uint32_t timeout = (uint32_t)-1
		) {
			return impl_post(url, args, result_code, timeout);
		}

		virtual std::string hook_get(
			std::string _url,
			http::http_code& result_code,
			uint32_t timeout = (uint32_t)-1
		) {
			return impl_get(_url, result_code, timeout);
		}

		//#ifndef __cplusplus_cli
		virtual bool hook_download_file(
			std::string url,
			std::string file_name,
			std::string args = std::string(),
			bool post = false,
			bool async = false,
			event_handler_progress __event_handler_progress = event_handler_progress(),
			event_handler_complete __event_handler_complete = event_handler_complete(),
			event_handler_fail __event_handler_fail = event_handler_fail(),
			uint32_t timeout = (uint32_t)-1
		) {
			this->_event_handler_complete = __event_handler_complete;
			this->_event_handler_fail = __event_handler_fail;
			this->_event_handler_progress = __event_handler_progress;

			bool retval = impl_download_file(
				url,
				file_name,
				args,
				post,
				async,
				__event_handler_progress,
				__event_handler_complete,
				__event_handler_fail,
				timeout
			);
			release_handlers();
			return retval;
		}

		static std::string args_to_content(std::map<std::string, std::string> _args) {
			std::vector<std::string> args_vector;

			std::for_each(_args.begin(), _args.end(),
				[&](const std::pair<std::string, std::string>& str) {
				args_vector.push_back(str.first + "=" + str.second);
			}
			);

			return std::string();
			//return wpp::string::join(args_vector, "&", true);
		}

		void set_busy(bool _in = true) {
			if (_in && !_is_busy)
				new_request_cleanup();
			_is_busy = _in;
		}

		void new_request_cleanup() {
			_cancel = false;
		}

	protected:
		std::map < std::string, std::string > headers;
	public:
		bool is_busy() {
			return _is_busy;
		}

		virtual void set_header(std::string header_name, std::string header_value) {
			headers[header_name] = header_value;
		}

		virtual std::string get_header(std::string header_name) {
			return std::string();
			//return wpp::algorithm::map::map_value(headers, header_name, std::string(""));
		}

		virtual std::map<std::string, std::string> get_headers() {
			return headers;
		}

		virtual void flush_headers() {
			headers.clear();
		}

		virtual bool has_header(std::string header_name) {
			return headers.find(header_name) != headers.end();
		}

		/*
		async = true, non blocking
		*/

		virtual bool download_file(
			std::string url,
			std::string file_path,
			std::string args = std::string(),
			bool post = false,
			bool async = false,
			event_handler_progress __event_handler_progress = event_handler_progress(),
			event_handler_complete __event_handler_complete = event_handler_complete(),
			event_handler_fail __event_handler_fail = event_handler_fail(),
			uint32_t timeout = (uint32_t)-1
		) {
			//std::cout << "\n" << __FUNCTION__ << "  " << __FILE__ << " | " << __LINE__;
			set_busy(true);
			auto retval = hook_download_file(
				url,
				file_path,
				args,
				post,
				async,
				__event_handler_progress,
				__event_handler_complete,
				__event_handler_fail,
				timeout
			);
			//std::cout << "\n" << __FUNCTION__ << "  " << __FILE__ << " | " << __LINE__;

			if (!async)
				set_busy(false);

			//std::cout << "\n" << __FUNCTION__ << "  " << __FILE__ << " | " << __LINE__;
			return retval;
		}

		virtual void stop() {
			_cancel = true;
		}

		virtual http_async_download_info fetch_async_info() {
			std::cout << "\n" << __FUNCTION__ << "  " << __FILE__ << " | " << __LINE__;
			return this->async_info;
		}

		virtual std::string post(
			std::string url,
			std::string content,
			http::http_code& result_code,
			uint32_t timeout = (uint32_t)-1
		) {
			std::string retval;
			set_busy(true);
			retval = hook_post(url, content, result_code, timeout);
			set_busy(false);
			return retval;
		}

		virtual std::string post(
			std::string url,
			std::map<std::string, std::string> args,
			http::http_code& result_code,
			uint32_t timeout = (uint32_t)-1
		) {
			set_busy(true);
			std::string retval = hook_post(url, args, result_code, timeout);
			set_busy(false);
			return retval;
		}

		virtual std::string get(
			std::string _url,
			http::http_code& result_code,
			uint32_t timeout
		) {
			set_busy(true);
			std::string retval = hook_get(_url, result_code, timeout);
			set_busy(false);
			return retval;
		}

		virtual std::string post(
			std::string _url,
			std::string content,
			uint32_t timeout = (uint32_t)-1
		) {
			http::http_code _error;
			set_busy(true);
			std::string query_content = post(_url, content, _error, timeout);
			set_busy(false);
			if (_error != http::http_code::http_response_ok) {
				throw http_exception(_error);
			}
			return query_content;
		}

		virtual std::string post(
			std::string _url,
			std::map<std::string, std::string> args,
			uint32_t timeout = (uint32_t)-1
		) {
			http::http_code _error;
			set_busy(true);
			std::string query_content = post(_url, args, _error, timeout);
			set_busy(false);
			if (_error != http::http_code::http_response_ok) {
				throw http_exception(_error);
			}
			return query_content;
		}

		virtual std::string get(
			std::string _url,
			uint32_t timeout = (uint32_t)-1
		) {
			http::http_code _error;
			set_busy(true);
			std::string query_content = get(_url, _error, timeout);
			set_busy(false);
			if (_error != http::http_code::http_response_ok) {
				throw http_exception(_error);
			}
			return query_content;
		}

		virtual std::string do_request(
			http::http_code& _error,
			std::string _url,
			bool post = false,
			std::map<std::string, std::string> args = std::map<std::string, std::string>(),
			uint32_t timeout = (uint32_t)-1
		) {
			std::string retval;
			set_busy(true);
			if (post) {
				retval = this->post(_url, args, _error, timeout);
			}
			else {
				retval = this->get(merge_args_url(_url, args), _error, timeout);
			}
			set_busy(false);
			return retval;
		}

		virtual std::string do_request(
			std::string _url,
			bool post = false,
			std::map<std::string, std::string> args = std::map<std::string, std::string>(),
			uint32_t timeout = (uint32_t)-1
		) {
			std::string retval;
			http::http_code _error = http::http_code::http_response_ok;
			set_busy(true);
			if (post) {
				retval = this->post(_url, args, _error);
			}
			else {
				retval = this->get(merge_args_url(_url, args), _error);
			}
			set_busy(false);
			if (_error != http::http_code::http_response_ok) {
				throw http_exception(_error);
			}
			return retval;
		}


		static client_base* get() {
			static client_base* m = nullptr;
			if (!m)
				m = new client_base();
			return m;
		}
	};
}