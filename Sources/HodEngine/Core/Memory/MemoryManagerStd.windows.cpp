#include "HodEngine/Core/pch.hpp"
#include "HodEngine/Core/Memory/MemoryManagerStd.hpp"

#if !defined(HOD_ENABLED_MEMLEAK_DETECTOR)

#include <cstdlib>
#include <malloc.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace hod
{
	/// @brief 
	/// @param size 
	/// @param alignment 
	/// @return 
	void* MemoryManagerStd::AllocateAlign(uint32_t size, uint32_t alignment)
	{
		return _aligned_malloc(size, alignment);
	}

	/// @brief 
	/// @param ptr 
	void MemoryManagerStd::FreeAlign(void* ptr, uint32_t alignment)
	{
		_aligned_free(ptr);
	}
}

#endif
