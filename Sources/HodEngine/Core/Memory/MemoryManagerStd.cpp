#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Memory/MemoryManagerStd.hpp"

#if !defined(HOD_ENABLED_MEMLEAK_DETECTOR)

#include <cstdlib>

namespace hod
{
	/// @brief 
	/// @param size 
	/// @return 
	void* MemoryManagerStd::Allocate(uint32_t size)
	{
		return malloc(size);
	}

	/// @brief 
	/// @param ptr 
	void MemoryManagerStd::Free(void* ptr)
	{
		free(ptr);
	}
}

#endif
