#pragma once
#include "HodEngine/Core/Export.hpp"
#include "HodEngine/Core/Memory/Config.hpp"

#if defined(HOD_ENABLED_MEMLEAK_DETECTOR)

	#include "HodEngine/Core/Concurency/SpinLock.hpp"
	#include "HodEngine/Core/Memory/Allocator.hpp"
	#include "HodEngine/Core/Memory/MallocAllocator.hpp"

	#include <array>
	#include <cstdint>

namespace hod
{
	class HOD_CORE_API MemLeakDetectorAllocator : public Allocator
	{
	public:
		struct AllocationHeader
		{
			AllocationHeader* _next = nullptr;
			AllocationHeader* _prev = nullptr;
			uint32_t          _size = 0;

			std::array<void*, 64> _callstack;
			uint32_t              _callstackSize = 0;
		};

	public:
		~MemLeakDetectorAllocator() override;

		void* AllocateInternal(uint32_t size, uint32_t alignment) override;
		void* ReallocateInternal(void* ptr, uint32_t newSize, uint32_t alignment) override;
		bool  ResizeInternal(void* ptr, uint32_t newSize, uint32_t alignment) override;
		void  FreeInternal(void* ptr) override;

	private:
		bool DumpReport();

	private:
		SpinLock _spinLock;
		bool     _stopAllocationCollect = false;

		AllocationHeader* _firstAlloc = nullptr;
		AllocationHeader* _lastAlloc = nullptr;

		MallocAllocator _mallocAllocator;
	};
}

#endif
