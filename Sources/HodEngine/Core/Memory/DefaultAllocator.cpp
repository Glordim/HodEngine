#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"

#include "HodEngine/Core/Memory/Config.hpp"
#include "HodEngine/Core/Memory/MallocAllocator.hpp"
#include "HodEngine/Core/Memory/MemLeakDetectorAllocator.hpp"

namespace hod
{
#if defined(HOD_ENABLED_MEMLEAK_DETECTOR)
	using DefaultAllocatorType = MemLeakDetectorAllocator;
#else
	using DefaultAllocatorType = MallocAllocator;
#endif

#if defined(_MSC_VER)
	#pragma warning(disable : 4075)
	#pragma init_seg(".CRT$XCA-000")
	DefaultAllocator DefaultAllocator::_defaultAllocator;
	#pragma warning(default : 4075)
#else
	DefaultAllocator DefaultAllocator::_defaultAllocator __attribute__((init_priority(101)));
#endif

	DefaultAllocator::DefaultAllocator()
	{
		DefaultAllocator::GetInstance();
	}

	Allocator& DefaultAllocator::GetInstance()
	{
		static DefaultAllocatorType defaultAllocatorInternal;
		return defaultAllocatorInternal;
	}
}
