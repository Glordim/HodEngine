#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/OS.hpp"

#include <Windows.h>

namespace hod
{
	/// @brief 
	/// @return 
	std::string OS::GetLastWin32ErrorMessage()
	{
		LPVOID lpMsgBuf;
		DWORD dw = GetLastError();

		::FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL);

		std::string message = (char*)lpMsgBuf;

		::LocalFree(lpMsgBuf);

		return message;
	}
}
