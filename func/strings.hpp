#ifndef _FUNCTIONS_STRINGS_HYZ
#define _FUNCTIONS_STRINGS_HYZ
#include <string>
#include <stdlib.h>
#include <time.h> 
#include <Windows.h>


// TODO - create .cpp for don't use inline

namespace hyz {
	inline std::wstring str2ws(const std::string& s)
	{
		int len;
		int slength = (int)s.length() + 1;
		len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
		wchar_t* buf = new wchar_t[len];
		MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
		std::wstring r(buf);
		delete[] buf;
		return r;
	}

	inline std::string rand2str(const char* lflag = "", const char* rflag = "")
	{
		// to remove warning
		srand(static_cast<UINT>(time(NULL)));
		return lflag + std::to_string((rand() % 100)) + rflag;
	}
}

#endif