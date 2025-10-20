#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/OS.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <DbgHelp.h>
#include <shellapi.h>

#include <HodEngine/Core/FileSystem/Path.hpp>

namespace hod
{
	/// @brief
	/// @param callstack
	/// @param maxSize
	/// @return
	uint32_t OS::GetCallstack(void** callstack, uint32_t maxSize)
	{
		return CaptureStackBackTrace(4, maxSize, callstack, nullptr);
	}

	/// @brief
	/// @param addr
	/// @return
	String OS::GetSymbol(void* addr)
	{
		String symbol;
		HANDLE hProcess = GetCurrentProcess();

		static bool symInitialized = false;
		if (symInitialized == false)
		{
			if (SymInitialize(hProcess, NULL, TRUE) == FALSE)
			{
				OUTPUT_ERROR("SymInitialize error : {}", OS::GetLastWin32ErrorMessage());
				return symbol; // assert ?
			}
			symInitialized = true;
		}

		constexpr uint32_t maxFunctionNameSize = 255;
		uint8_t            data[sizeof(SYMBOL_INFO) + maxFunctionNameSize * sizeof(uint8_t)];
		PSYMBOL_INFO       symbolInfo = reinterpret_cast<PSYMBOL_INFO>(data);
		symbolInfo->MaxNameLen = maxFunctionNameSize;
		symbolInfo->SizeOfStruct = sizeof(SYMBOL_INFO);

		if (SymFromAddr(hProcess, reinterpret_cast<DWORD64>(addr), 0, symbolInfo) == TRUE)
		{
			if (symbolInfo->NameLen >= symbolInfo->MaxNameLen)
			{
				symbolInfo->Name[symbolInfo->NameLen - 1] = '\0';
			}
			symbol = symbolInfo->Name;
		}
		else
		{
			symbol = "NOT_FOUND";
		}
		return symbol;
	}

	/// @brief
	/// @param addr
	/// @param symbolInfo
	/// @param demangle
	/// @return
	bool OS::GetSymbolInfo(void* addr, SymbolInfo& symbolInfo, bool demangle)
	{
		HANDLE hProcess = GetCurrentProcess();

		static bool symInitialized = false;
		if (symInitialized == false)
		{
			if (SymInitialize(hProcess, NULL, TRUE) == FALSE)
			{
				OUTPUT_ERROR("SymInitialize error : {}", OS::GetLastWin32ErrorMessage());
				return false;
			}
			symInitialized = true;
		}

		constexpr uint32_t maxFunctionNameSize = 1024;
		uint8_t            data[sizeof(SYMBOL_INFO) + maxFunctionNameSize];
		PSYMBOL_INFO       win32SymbolInfo = reinterpret_cast<PSYMBOL_INFO>(data);
		win32SymbolInfo->MaxNameLen = maxFunctionNameSize;
		win32SymbolInfo->SizeOfStruct = sizeof(SYMBOL_INFO);

		DWORD64 displacement = 0;
		if (SymFromAddr(hProcess, reinterpret_cast<DWORD64>(addr), &displacement, win32SymbolInfo) == FALSE)
		{
			return false;
		}

		if (win32SymbolInfo->NameLen >= win32SymbolInfo->MaxNameLen)
		{
			win32SymbolInfo->Name[win32SymbolInfo->NameLen - 1] = '\0';
		}

		IMAGEHLP_MODULE64 moduleInfo;
		memset(&moduleInfo, 0, sizeof(moduleInfo));
		moduleInfo.SizeOfStruct = sizeof(moduleInfo);

		if (SymGetModuleInfo64(hProcess, win32SymbolInfo->ModBase, &moduleInfo) == FALSE)
		{
			symbolInfo._module = "???";
		}
		else
		{
			Path modulePath(moduleInfo.ImageName);
			symbolInfo._module = modulePath.Filename().GetString();
		}

		symbolInfo._address = reinterpret_cast<void*>(win32SymbolInfo->Address);
		symbolInfo._function = win32SymbolInfo->Name;
		symbolInfo._line = static_cast<uint32_t>(displacement);

		if (demangle)
		{
		}

		return true;
	}

	/// @brief
	/// @return
	String OS::GetLastWin32ErrorMessage()
	{
		LPVOID lpMsgBuf;
		DWORD  dw = GetLastError();

		::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		                (LPTSTR)&lpMsgBuf, 0, NULL);

		String message = (char*)lpMsgBuf;

		::LocalFree(lpMsgBuf);

		return message;
	}

	/// @brief
	/// @param filePath
	bool OS::OpenFileWithDefaultApp(const char* filePath)
	{
		HINSTANCE result = ShellExecute(NULL, "open", filePath, NULL, NULL, SW_SHOWNORMAL);
		if ((INT_PTR)result <= 32)
		{
			OUTPUT_ERROR("OS::OpenFileWithDefaultApp: ShellExecute open {} fail ({})", filePath, OS::GetLastWin32ErrorMessage());
			return false;
		}
		return true;
	}
}
