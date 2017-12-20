#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "initializer\client_initializer.h"
#include "http\requester.h"

#include <thread>

int main(){
	initializer::client_initializer::get()->run();
	http::requester::get()->run();


	std::this_thread::sleep_for(std::chrono::milliseconds((uint32_t)-1));
    return 0;
}

