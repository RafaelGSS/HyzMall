#include "client_initializer.h"

using namespace initializer;

client_initializer::client_initializer()
{
}


client_initializer::~client_initializer()
{
}

void client_initializer::run()
{
	std::cout << "client::initializer::register client\n";
	register_client();
	// todo - create task windows
}


void client_initializer::register_client()
{
	uint32_t ec = http::http_code::http_ok;
	std::string sub_path = "user/register";
	auto body = http::http_request::get()->get_basic_body();
	http::http_request::get()->post_request(sub_path,body.dump(), ec);

	if (ec) {
		std::cout << "client::initializer error in request " << ec << __FUNCTION__ << '\n';
	}
		
}

client_initializer* client_initializer::get()
{
	static client_initializer* singleton = nullptr;
	if (!singleton)
		singleton = new client_initializer();
	return singleton;
}

