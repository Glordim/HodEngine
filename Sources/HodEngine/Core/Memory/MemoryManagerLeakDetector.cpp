#include "HodEngine/Core/pch.hpp"
#include "HodEngine/Core/Memory/MemoryManagerLeakDetector.hpp"

#if defined(HOD_ENABLED_MEMLEAK_DETECTOR)

#include <cstdlib>
#include <malloc.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <DbgHelp.h>

namespace hod
{
    /// @brief 
    MemoryManagerLeakDetector::~MemoryManagerLeakDetector()
	{
		uint32_t memleakCount = _allocationCount;
		if (memleakCount > 0)
		{
			HANDLE hProcess = GetCurrentProcess();
			SymInitialize(hProcess, NULL, TRUE);

			FILE* memleakReport = fopen("MemleakReport.txt", "w");
			fprintf(memleakReport, "Memleak count = %u\n\n", memleakCount);

			for (uint32_t index = 0; index < memleakCount; ++index)
			{
				const Allocation& allocation = _allocations[index];

				fprintf(memleakReport, "Ptr = %p\n", allocation._ptr);
				fprintf(memleakReport, "Size = %u\n", allocation._size);
				fprintf(memleakReport, "Callstack :\n");

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
						fprintf(memleakReport, "\t%s\n", symbol->Name);
					}
					else
					{
						fprintf(memleakReport, "\tNOT_FOUND\n");
					}
				}
				fprintf(memleakReport, "\n\n");
			}
			fclose(memleakReport);
		}
	}

    /// @brief 
    /// @param size 
    /// @return 
    void* MemoryManagerLeakDetector::Allocate(uint32_t size)
    {
		void* ptr = malloc(size);
		InsertAllocation(ptr, size);
        return ptr;
    }

	/// @brief 
    /// @param size 
    /// @param alignment 
    /// @return 
    void* MemoryManagerLeakDetector::AllocateAlign(uint32_t size, uint32_t alignment)
    {
		void* ptr = _aligned_malloc(size, alignment);
		InsertAllocation(ptr, size);
        return ptr;
    }

    /// @brief 
    /// @param ptr 
    void MemoryManagerLeakDetector::Free(void* ptr)
    {
		RemoveAllocation(ptr);
		free(ptr);
    }

	/// @brief 
    /// @param ptr 
    void MemoryManagerLeakDetector::FreeAlign(void* ptr, uint32_t alignment)
    {
		RemoveAllocation(ptr);
		_aligned_free(ptr);
    }

	/// @brief 
	/// @param ptr 
	/// @param size 
	void MemoryManagerLeakDetector::InsertAllocation(void* ptr, uint32_t size)
	{
		Allocation allocation;
        allocation._callstackSize = CaptureStackBackTrace(3, 64, allocation._callstack.data(), nullptr);
        allocation._size = size;
        allocation._ptr = ptr;

		_mutex.lock();
        _allocations[_allocationCount] = allocation;
		++_allocationCount;
		_mutex.unlock();
	}

    /// @brief 
    /// @param ptr 
    void MemoryManagerLeakDetector::RemoveAllocation(void* ptr)
	{
		_mutex.lock();
		for (uint32_t index = 0; index < _allocationCount; ++index)
		{
			if (_allocations[index]._ptr == ptr)
			{
				if (index != _allocationCount - 1)
				{
					std::swap(_allocations[index], _allocations[_allocationCount - 1]);
				}
				--_allocationCount;
				break;
			}
		}
		_mutex.unlock();
	}
}

#endif
