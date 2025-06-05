#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Memory/MallocAllocator.hpp"

#include <cstdlib>

namespace hod
{
	void* MallocAllocator::Allocate(uint32_t size)
	{
		return std::malloc(size);
	}

	void* MallocAllocator::Allocate(uint32_t size, uint32_t alignment)
	{
#if PLATFORM_WINDOWS
		return _aligned_malloc(size, alignment);
#else
		return std::aligned_alloc(alignment, size);
#endif
	}

	void* MallocAllocator::Reallocate(void* ptr, uint32_t newSize)
	{
		return std::realloc(ptr, newSize);
	}

	void* MallocAllocator::Reallocate(void* ptr, uint32_t newSize, uint32_t alignment)
	{
		if (ptr == nullptr)
			return Allocate(newSize, alignment);

		return std::realloc(ptr, newSize);
	}

	void MallocAllocator::Free(void* ptr)
	{
		std::free(ptr);
	}

	void MallocAllocator::Free(void* ptr, uint32_t alignment)
	{
#if PLATFORM_WINDOWS
		return _aligned_free(ptr);
#else
		return std::free(ptr);
#endif
	}
}
