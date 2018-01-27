#pragma once
#include <string>
#include <chrono>

#include <boost\asio\io_service.hpp>
#include <boost\asio\ip\udp.hpp>
#include <boost\asio\connect.hpp>
#include <boost\array.hpp>

#include <task\base\base_task.h>


class socket_client : public client::base::base_task
{
	boost::asio::io_service io_service;
	boost::asio::ip::udp::socket socket_udp;
	boost::system::error_code error;

	boost::array<char, 1024> buffer;

	uint16_t port;
	std::string ip;


	void set_config(uint16_t _port, std::string _ip);
	bool open_socket_udp();
	void send_packets(uint32_t seconds);
public:
	bool drop_connection(std::string port, std::string ip, std::string seconds);
	void execute();
	void on_execute();
	void run(
		std::string id,
		bool response,
		std::string method,
		std::vector<std::string> args
	);
	_function fetch_function(std::string);

	socket_client() : socket_udp(io_service) {}
};

