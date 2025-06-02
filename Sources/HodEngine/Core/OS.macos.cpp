#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/OS.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include <execinfo.h>
#include <cxxabi.h>

#include <cassert>
#include <cstring>

namespace hod
{
	/// @brief 
	/// @param callstack 
	/// @param maxSize 
	/// @return 
	uint32_t OS::GetCallstack(void** callstack, uint32_t maxSize)
	{
		return backtrace(callstack, maxSize);
	}

	/// @brief 
	/// @param addr 
	/// @return 
	std::string OS::GetSymbol(void* addr)
	{
		std::string symbol = "NOT_FOUND";

		char** symbols = backtrace_symbols(&addr, 1);
		if (symbols == nullptr)
		{
			perror("backtrace_symbols");
			exit(EXIT_FAILURE);
		}
		else
		{
			symbol = symbols[0];
			free(symbols);
		}
		
		return symbol;
	}

	/// @brief 
	/// @param addr 
	/// @param symbol 
	/// @param size 
	/// @return 
	bool OS::GetSymbolInfo(void* addr, SymbolInfo& symbolInfo, bool demangle)
	{
		char** symbols = backtrace_symbols(&addr, 1);
		if (symbols == nullptr)
		{
			perror("backtrace_symbols");
			exit(EXIT_FAILURE);
		}
		else
		{
			// example : 0   libWindow.dylib                     0x00000001000bbda4 _ZN3hod6window11MacOsWindowC2Eb + 376

			const char* it = symbols[0];
			while (*it != '\0')
				++it;

			const char* mangledEnd = it;
			while (*it != '+' && it != symbols[0])
				--it;

			if (*it == '+')
			{
				mangledEnd = it - 2;
			}

			const char* mangledStart = symbols[0];
			it = mangledEnd;
			while (*it != ' ' && it != symbols[0])
				--it;

			if (*it == ' ')
			{
				mangledStart = it + 1;
			}

			char mangledSymbol[512];
			std::strncpy(mangledSymbol, mangledStart, (mangledEnd - mangledStart) + 1);
			mangledSymbol[(mangledEnd - mangledStart) + 1] = '\0';

			int status;
			char* demangledSymbol = abi::__cxa_demangle(mangledSymbol, nullptr, nullptr, &status);
			if (status == 0)
			{
				std::strncpy(symbol, demangledSymbol, size);
				symbol[512 - 1] = '\0';
				free(demangledSymbol);
			}
			else
			{
				std::strncpy(symbol, symbols[0], size);
				symbol[512 - 1] = '\0';
			}
			free(symbols);
		}
		
		return symbol;
	}
}
