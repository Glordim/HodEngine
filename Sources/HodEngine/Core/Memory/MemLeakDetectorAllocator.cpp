#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Memory/MemLeakDetectorAllocator.hpp"
#include "HodEngine/Core/OS.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"

#if defined(HOD_ENABLED_MEMLEAK_DETECTOR)

#include <cstdlib>
#include <cstring>
#include <cassert>

namespace hod
{
	/// @brief 
	MemLeakDetectorAllocator::~MemLeakDetectorAllocator()
	{
		DumpReport();
	}

	/// @brief 
	/// @param size 
	/// @return 
	void* MemLeakDetectorAllocator::Allocate(uint32_t size)
	{
		return Allocate(size, alignof(std::max_align_t));
	}

	/// @brief 
	/// @param size 
	/// @param alignment 
	/// @return 
	void* MemLeakDetectorAllocator::Allocate(uint32_t size, uint32_t alignment)
	{
		if (alignment < alignof(std::max_align_t) || std::has_single_bit(alignment) == false) // check if pow2
		{
			assert(false);
			alignment = alignof(std::max_align_t);
		}

		uint32_t maxPadding  = 0;
		if (alignment > 0)
		{
			maxPadding  = alignment - 1;
		}
		
		// [ AllocationHeader ][ padding ][ pointer to header ][ aligned user ptr ]
		void* allocation = _mallocAllocator.Allocate(sizeof(AllocationHeader) + maxPadding  + sizeof(AllocationHeader*) + size);
		if (allocation == nullptr)
		{
			return nullptr;
		}

		AllocationHeader* allocationHeader = static_cast<AllocationHeader*>(allocation);
		allocationHeader->_callstackSize = OS::GetCallstack(allocationHeader->_callstack.data(), (uint32_t)allocationHeader->_callstack.size());
		allocationHeader->_size = size;
		allocationHeader->_next = nullptr;
		if (_stopAllocationCollect == false)
		{
			_spinLock.Lock();
			allocationHeader->_prev = _lastAlloc;
			if (_firstAlloc == nullptr)
			{
				_firstAlloc = allocationHeader;
			}
			else if (_lastAlloc != nullptr)
			{
				_lastAlloc->_next = allocationHeader;
			}
			_lastAlloc = allocationHeader;
			_spinLock.Unlock();
		}

		uintptr_t rawAddr = reinterpret_cast<uintptr_t>(allocation) + sizeof(AllocationHeader) + sizeof(AllocationHeader*);
		uintptr_t alignedAddr = (rawAddr + alignment - 1) & ~(static_cast<uintptr_t>(alignment) - 1);

		AllocationHeader** pointerToHeader = reinterpret_cast<AllocationHeader**>(alignedAddr - sizeof(AllocationHeader*));
		*pointerToHeader = allocationHeader;

		void* alignedPtr = reinterpret_cast<void*>(alignedAddr);
		std::memset(alignedPtr, 0xA1, size);
		return alignedPtr;
	}

	/// @brief 
	/// @param ptr 
	/// @param newSize 
	/// @return 
	void* MemLeakDetectorAllocator::Reallocate(void* ptr, uint32_t newSize)
	{
		return nullptr; // todo
	}

	/// @brief 
	/// @param ptr 
	/// @param newSize 
	/// @param alignment 
	/// @return 
	void* MemLeakDetectorAllocator::Reallocate(void* ptr, uint32_t newSize, uint32_t alignment)
	{
		return nullptr; // todo
	}

	/// @brief 
	/// @param userAddress 
	void MemLeakDetectorAllocator::Free(void* userAddress)
	{
		if (userAddress == nullptr)
		{
			return;
		}

		AllocationHeader* allocationHeader = *reinterpret_cast<AllocationHeader**>((reinterpret_cast<uintptr_t>(userAddress) - sizeof(AllocationHeader*)));
		if (_stopAllocationCollect == false)
		{
			_spinLock.Lock();
			if (allocationHeader->_next != nullptr)
			{
				allocationHeader->_next->_prev = allocationHeader->_prev;
			}
			if (allocationHeader->_prev != nullptr)
			{
				allocationHeader->_prev->_next = allocationHeader->_next;
			}
			if (_firstAlloc == allocationHeader)
			{
				_firstAlloc = allocationHeader->_next;
			}
			if (_lastAlloc == allocationHeader)
			{
				_lastAlloc = allocationHeader->_prev;
			}
			_spinLock.Unlock();
		}

		std::memset(userAddress, 0xDE, allocationHeader->_size);
		_mallocAllocator.Free(allocationHeader);
	}

	/// @brief 
	/// @param ptr 
	void MemLeakDetectorAllocator::Free(void* ptr, uint32_t alignment)
	{
		if (ptr == nullptr)
		{
			return;
		}
		
		(void)alignment;
		Free(ptr);
	}

	/// @brief 
	/// @return 
	bool MemLeakDetectorAllocator::DumpReport()
	{
		bool result = true;

		new float();

		_spinLock.Lock();
		_stopAllocationCollect = true;
		if (_firstAlloc != nullptr)
		{
			SymbolInfo symbolInfo;
			symbolInfo._function.reserve(2048);
			symbolInfo._module.reserve(2048);

			uint32_t totalLeak = 0;
			uint64_t totalSize = 0;
			AllocationHeader* it = _firstAlloc;
			while (it != nullptr)
			{
				++totalLeak;
				totalSize += it->_size;
				it = it->_next;
			}

			std::filesystem::path memleakReportPath = FileSystem::GetTemporaryPath() / "HodEngine" / "MemleakReport.txt";
			std::filesystem::create_directories(memleakReportPath.parent_path());
			FILE* memleakReport = fopen(memleakReportPath.string().c_str(), "w");
			if (memleakReport == NULL)
			{
				OUTPUT_ERROR("MemoryManagerLeakDetector: Fail to open MemleakReport.txt : {}", OS::GetLastWin32ErrorMessage());
				result = false;
			}
			else
			{
				fprintf(memleakReport, "Leak = %u\n", totalLeak);
				fprintf(memleakReport, "Size = %ju\n\n", totalSize);

				it = _firstAlloc;
				while (it != nullptr)
				{
					//fprintf(memleakReport, "Ptr = %p\n", it->_userAddress);
					fprintf(memleakReport, "Size = %u\n", it->_size);
					fprintf(memleakReport, "Callstack :\n");

					for (uint32_t index = 0; index < it->_callstackSize; ++index)
					{
						OS::GetSymbolInfo(it->_callstack[index], symbolInfo, true);
						fprintf(memleakReport, "\t%-*s %s + %u\n", 24, symbolInfo._module.c_str(), symbolInfo._function.c_str(), symbolInfo._line);
					}

					fprintf(memleakReport, "\n\n");

					it = it->_next;
				}

				fclose(memleakReport);

				OS::OpenFileWithDefaultApp(memleakReportPath.string().c_str());
			}
		}
		_stopAllocationCollect = false;
		_spinLock.Unlock();
		return result;
	}
}

#endif
