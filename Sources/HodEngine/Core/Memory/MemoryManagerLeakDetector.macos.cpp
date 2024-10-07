#include "HodEngine/Core/pch.hpp"
#include "HodEngine/Core/Memory/MemoryManagerLeakDetector.hpp"

#if defined(HOD_ENABLED_MEMLEAK_DETECTOR)
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>

#include <cassert>

namespace hod
{
	/// @brief 
	/// @param report 
	/// @param allocation 
	void MemoryManagerLeakDetector::WriteCallstackInReport(FILE* report, const Allocation& allocation) const
	{
		char** symbols = backtrace_symbols(allocation._callstack.data(), allocation._callstackSize);

		if (symbols == NULL)
		{
			perror("backtrace_symbols");
			exit(EXIT_FAILURE);
		}

		for (uint32_t currentFrame = 0; currentFrame < allocation._callstackSize; ++currentFrame)
		{
			fprintf(report, "\t%s\n", symbols[currentFrame]);
		}
	}

    /// @brief 
    /// @param callstack 
    /// @return 
    uint32_t MemoryManagerLeakDetector::GetCallstack(std::array<void*, 64>& callstack) const
	{
		return backtrace(callstack.data(), callstack.size());
	}

	/// @brief 
    /// @param size 
    /// @param alignment 
    /// @return 
    void* MemoryManagerLeakDetector::AllocateAlign(uint32_t size, uint32_t alignment)
    {
		void* ptr = nullptr;
		int result = posix_memalign(&ptr, alignment, size);
		if (result != 0)
		{
			return nullptr;
		}
		InsertAllocation(ptr, size);
        return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(ptr) + sizeof(Allocation));
    }

	/// @brief 
    /// @param ptr 
    void MemoryManagerLeakDetector::FreeAlign(void* ptr, uint32_t alignment)
    {
		if (ptr == nullptr)
		{
			return;
		}
		
		RemoveAllocation(ptr);
		free(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(ptr) - sizeof(Allocation)));
    }
}

#endif
