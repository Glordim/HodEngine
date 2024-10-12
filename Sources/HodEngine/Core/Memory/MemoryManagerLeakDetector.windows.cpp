#include "HodEngine/Core/pch.hpp"
#include "HodEngine/Core/Memory/MemoryManagerLeakDetector.hpp"

#if defined(HOD_ENABLED_MEMLEAK_DETECTOR)

#include <cstdlib>
#include <malloc.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <DbgHelp.h>

#include <cassert>

namespace hod
{
	/// @brief 
	/// @param report 
	/// @param allocation 
	void MemoryManagerLeakDetector::WriteCallstackInReport(FILE* report, const Allocation& allocation) const
	{
		HANDLE hProcess = GetCurrentProcess();
		SymInitialize(hProcess, NULL, TRUE);

		constexpr uint32_t maxFunctionNameSize = 255;
		uint8_t data[sizeof(SYMBOL_INFO) + maxFunctionNameSize * sizeof(uint8_t)];
		PSYMBOL_INFO symbol = reinterpret_cast<PSYMBOL_INFO>(data);
		symbol->MaxNameLen = maxFunctionNameSize;
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

		for (uint32_t currentFrame = 0; currentFrame < allocation._callstackSize; ++currentFrame)
		{
			if (SymFromAddr(hProcess, reinterpret_cast<DWORD64>(allocation._callstack[currentFrame]), 0, symbol) == TRUE)
			{
				if (symbol->NameLen == symbol->MaxNameLen)
				{
					symbol->Name[symbol->NameLen - 1] = '\0';
				}
				fprintf(report, "\t%s\n", symbol->Name);
			}
			else
			{
				fprintf(report, "\tNOT_FOUND\n");
			}
		}
	}

    /// @brief 
    /// @param callstack 
    /// @return 
    uint32_t MemoryManagerLeakDetector::GetCallstack(std::array<void*, 64>& callstack) const
	{
		return CaptureStackBackTrace(4, (DWORD)callstack.size(), callstack.data(), nullptr);
	}
}

#endif
