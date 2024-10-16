#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/OS.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include <execinfo.h>

#include <cassert>

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
		}
		
		return symbol;
	}
}
