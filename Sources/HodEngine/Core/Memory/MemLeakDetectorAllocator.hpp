#pragma once
#include "HodEngine/Core/Export.hpp"
#include "HodEngine/Core/Memory/Config.hpp"

#if defined(HOD_ENABLED_MEMLEAK_DETECTOR)

#include "HodEngine/Core/Memory/Allocator.hpp"
#include "HodEngine/Core/Memory/MallocAllocator.hpp"
#include "HodEngine/Core/Concurency/SpinLock.hpp"

#include <cstdint>
#include <unordered_map>
#include <array>
#include <mutex>

namespace hod
{
	class HOD_CORE_API MemLeakDetectorAllocator : public Allocator
	{
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

		~MemLeakDetectorAllocator() override;

		[[nodiscard]] void*	Allocate(uint32_t size) override;
		[[nodiscard]] void*	Allocate(uint32_t size, uint32_t alignment) override;

		[[nodiscard]] void*	Reallocate(void* ptr, uint32_t newSize) override;
		[[nodiscard]] void*	Reallocate(void* ptr, uint32_t newSize, uint32_t alignment) override;

		void	Free(void* ptr) override;
		void	Free(void* ptr, uint32_t alignment) override;

	private:

		bool	DumpReport();

	private:

		SpinLock _spinLock;
		bool	 _stopAllocationCollect = false;
		uint32_t _allocationCount = 0;

		AllocationHeader*	_firstAlloc = nullptr;
		AllocationHeader*	_lastAlloc = nullptr;

		MallocAllocator	_mallocAllocator;
	};
}

#endif
