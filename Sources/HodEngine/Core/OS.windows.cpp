#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/OS.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <DbgHelp.h>

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
	std::string OS::GetSymbol(void* addr)
	{
		std::string symbol;
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
		uint8_t data[sizeof(SYMBOL_INFO) + maxFunctionNameSize * sizeof(uint8_t)];
		PSYMBOL_INFO symbolInfo = reinterpret_cast<PSYMBOL_INFO>(data);
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
