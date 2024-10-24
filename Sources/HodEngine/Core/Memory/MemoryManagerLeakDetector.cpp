#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Memory/MemoryManagerLeakDetector.hpp"
#include "HodEngine/Core/OS.hpp"

#if defined(HOD_ENABLED_MEMLEAK_DETECTOR)

#include <cstdlib>
#include <cstring>
#include <cassert>

namespace hod
{
    /// @brief 
    MemoryManagerLeakDetector::~MemoryManagerLeakDetector()
	{
		uint32_t memleakCount = _allocationCount;
		if (memleakCount > 0)
		{
			uint32_t size = 0;
			for (uint32_t index = 0; index < memleakCount; ++index)
			{
				const Allocation* allocation = _allocations[index];
				size += allocation->_size;
			}

			FILE* memleakReport = fopen("MemleakReport.txt", "w");
			fprintf(memleakReport, "Memleak count = %u\n", memleakCount);
			fprintf(memleakReport, "Size = %u\n\n", size);

			for (uint32_t index = 0; index < memleakCount; ++index)
			{
				const Allocation* allocation = _allocations[index];

				fprintf(memleakReport, "Ptr = %p\n", allocation->_userAddress);
				fprintf(memleakReport, "Size = %u\n", allocation->_size);
				fprintf(memleakReport, "Callstack :\n");

				for (uint32_t index = 0; index < allocation->_callstackSize; ++index)
				{
					fprintf(memleakReport, "\t%s\n", OS::GetSymbol(allocation->_callstack[index]).c_str());
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
		void* realAddress = malloc(size + sizeof(Allocation));
		void* userAddress = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(realAddress) + sizeof(Allocation));
		InsertAllocation(userAddress, size, realAddress);
        return userAddress;
    }

    /// @brief 
    /// @param userAddress 
    void MemoryManagerLeakDetector::Free(void* userAddress)
    {
		if (userAddress == nullptr)
		{
			return;
		}

		void* realAddress = RemoveAllocation(userAddress);
		free(realAddress);
    }

	/// @brief 
    /// @param size 
    /// @param alignment 
    /// @return 
    void* MemoryManagerLeakDetector::AllocateAlign(uint32_t size, uint32_t alignment)
    {
		void* realAddress = malloc(size + alignment - 1 + sizeof(Allocation));

		uintptr_t alignedAddressUInt = reinterpret_cast<uintptr_t>(realAddress) + sizeof(Allocation);
		alignedAddressUInt = (alignedAddressUInt + alignment - 1) & ~((uintptr_t)alignment - 1);

		void* alignedAddress = reinterpret_cast<void*>(alignedAddressUInt);
		assert(reinterpret_cast<uintptr_t>(alignedAddress) % alignment == 0);

		InsertAllocation(alignedAddress, size, realAddress);
		return alignedAddress;
    }

	/// @brief 
    /// @param ptr 
    void MemoryManagerLeakDetector::FreeAlign(void* ptr, uint32_t alignment)
    {
		if (ptr == nullptr)
		{
			return;
		}
		
		void* realAddress = RemoveAllocation(ptr);
		free(realAddress);
    }

	/// @brief 
	/// @param userAddress 
	/// @param size 
	/// @param address 
	void MemoryManagerLeakDetector::InsertAllocation(void* userAddress, uint32_t size, void* address)
	{
		uint32_t reservedSize = (uint32_t)(reinterpret_cast<uintptr_t>(userAddress) - reinterpret_cast<uintptr_t>(address));
		std::memset(address, 0xCE, reservedSize);
		std::memset(userAddress, 0xA1, size);

		Allocation* allocation = reinterpret_cast<Allocation*>(reinterpret_cast<uintptr_t>(userAddress) - sizeof(Allocation));
        allocation->_callstackSize = OS::GetCallstack(allocation->_callstack.data(), (uint32_t)allocation->_callstack.size());
        allocation->_size = size;
        allocation->_realAddress = address;
		allocation->_userAddress = userAddress;

		_spinLock.Lock();
		allocation->_index = _allocationCount;
        _allocations[_allocationCount] = allocation;
		++_allocationCount;
		_spinLock.Unlock();
	}

    /// @brief 
    /// @param userAddress 
    /// @return 
    void* MemoryManagerLeakDetector::RemoveAllocation(void* userAddress)
	{
		void* allocationAddress = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(userAddress) - sizeof(Allocation));
		Allocation* allocation = static_cast<Allocation*>(allocationAddress);
		assert(allocation->_userAddress == userAddress);

		_spinLock.Lock();
		if (allocation->_index != _allocationCount - 1)
		{
			assert(allocation->_index < _allocationCount);
			_allocations[_allocationCount - 1]->_index = allocation->_index;
			std::swap(_allocations[allocation->_index], _allocations[_allocationCount - 1]);
		}
		_allocations[_allocationCount - 1] = nullptr;
		--_allocationCount;
		_spinLock.Unlock();

		std::memset(userAddress, 0xDE, allocation->_size);
		return allocation->_realAddress;
	}
}

#endif
