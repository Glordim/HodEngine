#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/OS.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/StringConversion.hpp"

#include <string_view>
#include <win32/misc.h>
#include <win32/window.h>

#include <HodEngine/Core/FileSystem/Path.hpp>

//#include <windows.h>

extern "C"
{
	HINSTANCE ShellExecuteA(_In_opt_ HWND hwnd, _In_opt_ LPCSTR lpOperation, _In_ LPCSTR lpFile, _In_opt_ LPCSTR lpParameters, _In_opt_ LPCSTR lpDirectory, _In_ INT nShowCmd);
	HANDLE    LocalFree(HANDLE hMem);

	BOOL      IsClipboardFormatAvailable(_In_ UINT format);
	BOOL      OpenClipboard(_In_opt_ HWND hWndNewOwner);
	HANDLE    GetClipboardData(_In_ UINT uFormat);
	HANDLE    SetClipboardData(_In_ UINT uFormat, _In_opt_ HANDLE hMem);
	BOOL      EmptyClipboard(VOID);
	BOOL      CloseClipboard(VOID);

	#define CF_UNICODETEXT 13

	HGLOBAL   GlobalAlloc(_In_ UINT uFlags, _In_ SIZE_T dwBytes);
	LPVOID    GlobalLock(_In_ HGLOBAL hMem);
	BOOL      GlobalUnlock(_In_ HGLOBAL hMem);
	HGLOBAL   GlobalFree(HGLOBAL hMem);

	#define GMEM_MOVEABLE       0x0002
}

namespace hod::inline core
{
	/// @brief
	/// @return
	String OS::GetLastWin32ErrorMessage()
	{
		LPVOID lpMsgBuf;
		DWORD  dw = GetLastError();

		::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
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

	bool OS::SetEnv(const char* name, const char* value)
	{
		// todo A -> W

		if (SetEnvironmentVariableA(name, value) == FALSE)
		{
			OUTPUT_ERROR("OS::SetEnv: SetEnvironmentVariable set {}={} fail ({})", name, value, OS::GetLastWin32ErrorMessage());
			return false;
		}
		return true;
	}

	bool OS::ReadClipboard(String& value)
	{
		if (IsClipboardFormatAvailable(CF_UNICODETEXT) == FALSE || OpenClipboard(NULL) == FALSE)
			return false;

		bool success = false;
		HGLOBAL hGlobal = GetClipboardData(CF_UNICODETEXT);
		if (hGlobal != NULL)
		{
			const wchar_t* lpWStr = static_cast<const wchar_t*>(GlobalLock(hGlobal));
			if (lpWStr != NULL)
			{
				size_t len = std::char_traits<wchar_t>::length(lpWStr);
				size_t sizeNeeded = StringConversion::WStringToStringSize(lpWStr, len);
				if (sizeNeeded > 0)
				{
					value.Resize(sizeNeeded);
					success = StringConversion::WStringToString(lpWStr, len, value.Data(), sizeNeeded);
				}
				GlobalUnlock(hGlobal);
			}
		}
		CloseClipboard();
		return success;
	}

	bool OS::WriteClipboard(const std::string_view& value)
	{
		if (OpenClipboard(NULL) == FALSE)
			return false;

		bool success = false;
		size_t wLen = StringConversion::StringToWStringSize(value);
		HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, (wLen + 1) * sizeof(wchar_t));
		if (hGlobal != NULL)
		{
			wchar_t* lpStr = static_cast<wchar_t*>(GlobalLock(hGlobal));
			if (lpStr != NULL)
			{
				StringConversion::StringToWString(value, lpStr, wLen);
				lpStr[wLen] = L'\0';
				GlobalUnlock(hGlobal);
			}

			EmptyClipboard();
			success = SetClipboardData(CF_UNICODETEXT, hGlobal) != NULL;

			GlobalFree(hGlobal);
		}

		CloseClipboard();
		return success;
	}
}
