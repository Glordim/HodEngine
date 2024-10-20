#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/OS.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include <execinfo.h>
#include <cxxabi.h>

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
			char* mangledName = nullptr;
    	    char* offsetBegin = nullptr;
	        char* offsetEnd = nullptr;
			for (char* c = symbols[0]; *c; ++c)
			{
				if (*c == '(')
				{
					mangledName = c + 1;
				}
				else if (*c == '+')
				{
					offsetBegin = c;
				}
				else if (*c == ')')
				{
					offsetEnd = c;
					break;
				}
			}

			if (mangledName && offsetBegin && offsetEnd && mangledName < offsetBegin)
			{
				char demangledBuffer[512];
				size_t demangledBufferLen = sizeof(demangledBuffer);

				int status;
            	*offsetBegin = '\0';
				char* demangledSymbol = abi::__cxa_demangle(mangledName, demangledBuffer, &demangledBufferLen, &status);
				if (status == 0)
				{
					symbol = demangledSymbol;
				}
				else
				{
					symbol = symbols[0];
				}
			}
			else
			{
				symbol = symbols[0];
			}
        }
		
		return symbol;
	}
}
