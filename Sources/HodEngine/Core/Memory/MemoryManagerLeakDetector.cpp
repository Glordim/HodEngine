#include "HodEngine/Core/pch.hpp"
#include "HodEngine/Core/Memory/MemoryManagerLeakDetector.hpp"

#if defined(HOD_ENABLED_MEMLEAK_DETECTOR)

#include <cstdlib>

#include <cassert>

namespace hod
{
    /// @brief 
    MemoryManagerLeakDetector::~MemoryManagerLeakDetector()
	{
		uint32_t memleakCount = _allocationCount;
		if (memleakCount > 0)
		{
			FILE* memleakReport = fopen("MemleakReport.txt", "w");
			fprintf(memleakReport, "Memleak count = %u\n\n", memleakCount);

			for (uint32_t index = 0; index < memleakCount; ++index)
			{
				const Allocation* allocation = _allocations[index];

				fprintf(memleakReport, "Ptr = %p\n", allocation->_ptr);
				fprintf(memleakReport, "Size = %u\n", allocation->_size);
				fprintf(memleakReport, "Callstack :\n");

				WriteCallstackInReport(memleakReport, *allocation);

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
		void* ptr = malloc(size + sizeof(Allocation));
		InsertAllocation(ptr, size);
        return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(ptr) + sizeof(Allocation));
    }

    /// @brief 
    /// @param ptr 
    void MemoryManagerLeakDetector::Free(void* ptr)
    {
		if (ptr == nullptr)
		{
			return;
		}

		RemoveAllocation(ptr);
		free(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(ptr) - sizeof(Allocation)));
    }

	/// @brief 
	/// @param ptr 
	/// @param size 
	void MemoryManagerLeakDetector::InsertAllocation(void* ptr, uint32_t size)
	{
		Allocation* allocation = static_cast<Allocation*>(ptr);
        allocation->_callstackSize = GetCallstack(allocation->_callstack);
        allocation->_size = size;
        allocation->_ptr = ptr;

		std::memset(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(ptr) + sizeof(Allocation)), 0xA1, size);

		_mutex.lock();
		allocation->_index = _allocationCount;
        _allocations[_allocationCount] = allocation;
		++_allocationCount;
		_mutex.unlock();
	}

    /// @brief 
    /// @param ptr 
    void MemoryManagerLeakDetector::RemoveAllocation(void* ptr)
	{
		void* realPtr = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(ptr) - sizeof(Allocation));
		Allocation* allocation = static_cast<Allocation*>(realPtr);

		_mutex.lock();
		if (allocation->_index != _allocationCount - 1)
		{
			assert(allocation->_index < _allocationCount);
			_allocations[_allocationCount - 1]->_index = allocation->_index;
			std::swap(_allocations[allocation->_index], _allocations[_allocationCount - 1]);
		}
		_allocations[_allocationCount - 1] = nullptr;
		--_allocationCount;
		_mutex.unlock();

		std::memset(ptr, 0xDE, allocation->_size);
	}
}

#endif
