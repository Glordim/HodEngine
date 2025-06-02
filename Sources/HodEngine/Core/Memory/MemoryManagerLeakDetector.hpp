#pragma once
#include "HodEngine/Core/Export.hpp"
#include "HodEngine/Core/Memory/Config.hpp"
#include "HodEngine/Core/Concurency/SpinLock.hpp"

#if defined(HOD_ENABLED_MEMLEAK_DETECTOR)

#include <cstdint>
#include <unordered_map>
#include <array>
#include <mutex>

namespace hod
{
	class HOD_CORE_API MemoryManagerLeakDetector
	{
	public:

		static MemoryManagerLeakDetector _instance;
		static MemoryManagerLeakDetector* GetInstance() { return &_instance; }

	public:

		struct AllocationHeader
		{
			AllocationHeader*	_next = nullptr;
			AllocationHeader*	_prev = nullptr;
			uint32_t			_size = 0;

			std::array<void*, 64> _callstack;
			uint32_t _callstackSize = 0;
		};

	public:

		~MemoryManagerLeakDetector();

		void*       Allocate(uint32_t size);
		void*       AllocateAlign(uint32_t size, uint32_t alignment);

		void        Free(void* ptr);
		void        FreeAlign(void* ptr, uint32_t alignment);

	private:

		SpinLock _spinLock;
		bool	 _stopAllocationCollect = false;
		uint32_t _allocationCount = 0;

		AllocationHeader*	_firstAlloc = nullptr;
		AllocationHeader*	_lastAlloc = nullptr;
	};
}

#endif
