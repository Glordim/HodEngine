#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Memory/MallocAllocator.hpp"

#include <cstdlib>

namespace hod
{
	void* MallocAllocator::AllocateInternal(uint32_t size, uint32_t alignment)
	{
#if PLATFORM_WINDOWS
		return _aligned_malloc(size, alignment);
#else
		return std::aligned_alloc(alignment, size);
#endif
	}

	void* MallocAllocator::ReallocateInternal(void* ptr, uint32_t newSize, uint32_t alignment)
	{
		if (ptr == nullptr)
		{
			return Allocate(newSize, alignment);
		}

#if PLATFORM_WINDOWS
		return nullptr;
#else
		return std::realloc(ptr, newSize);
#endif
	}

	void MallocAllocator::FreeInternal(void* ptr)
	{
#if PLATFORM_WINDOWS
		return _aligned_free(ptr);
#else
		return std::free(ptr);
#endif
	}
}
