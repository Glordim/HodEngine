#include "HodEngine/Core/pch.hpp"
#include "HodEngine/Core/Memory/MemoryManagerStd.hpp"

#if !defined(HOD_ENABLED_MEMLEAK_DETECTOR)

#include <cstdlib>

namespace hod
{
	/// @brief 
	/// @param size 
	/// @param alignment 
	/// @return 
	void* MemoryManagerStd::AllocateAlign(uint32_t size, uint32_t alignment)
	{
		void* ptr;
		if (posix_memalign(&ptr, alignment, size) != 0)
		{
			return nullptr;
		}
		return ptr;
	}

	/// @brief 
	/// @param ptr 
	void MemoryManagerStd::FreeAlign(void* ptr, uint32_t alignment)
	{
		free(ptr);
	}
}

#endif
