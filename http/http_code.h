#pragma once
namespace http {
	enum http_code {
		http_ok = 200,
		http_not_found = 404,
		http_accepted = 202,
		http_server_error = 500
	};
}