#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/OS.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include <execinfo.h>
#include <cxxabi.h>
#include <dlfcn.h>

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
	String OS::GetSymbol(void* addr)
	{
		String symbol = "NOT_FOUND";

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
		Dl_info info;
		if (dladdr(addr, &info) == 0)
		{
			return false;
		}

		symbolInfo._address = info.dli_saddr;
		symbolInfo._module = info.dli_fname;
		symbolInfo._function = info.dli_sname;
		symbolInfo._line = 0;

		if (demangle)
		{
			int status;
			char* demangledSymbol = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
			if (status == 0)
			{
				symbolInfo._function = demangledSymbol;
				free(demangledSymbol);
			}
		}
		
		return true;
	}
}
