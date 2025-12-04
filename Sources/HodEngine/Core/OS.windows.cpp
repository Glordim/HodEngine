#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/OS.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include <win32/misc.h>
#include <win32/window.h>

#include <HodEngine/Core/FileSystem/Path.hpp>

extern "C"
{
	HINSTANCE ShellExecuteA(_In_opt_ HWND hwnd, _In_opt_ LPCSTR lpOperation, _In_ LPCSTR lpFile, _In_opt_ LPCSTR lpParameters, _In_opt_ LPCSTR lpDirectory, _In_ INT nShowCmd);
	HANDLE    LocalFree(HANDLE hMem);
}

namespace hod
{
	/// @brief
	/// @return
	String OS::GetLastWin32ErrorMessage()
	{
		LPVOID lpMsgBuf;
		DWORD  dw = GetLastError();

		::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		                 (LPTSTR)&lpMsgBuf, 0, NULL);

		String message = (char*)lpMsgBuf;

		::LocalFree(lpMsgBuf);

		return message;
	}

	/// @brief
	/// @param filePath
	bool OS::OpenFileWithDefaultApp(const char* filePath)
	{
		HINSTANCE result = ShellExecuteA(NULL, "open", filePath, NULL, NULL, SW_SHOWNORMAL);
		if ((INT_PTR)result <= 32)
		{
			OUTPUT_ERROR("OS::OpenFileWithDefaultApp: ShellExecute open {} fail ({})", filePath, OS::GetLastWin32ErrorMessage());
			return false;
		}
		return true;
	}
}
