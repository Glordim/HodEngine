#pragma once
#include "HodEngine/Core/Export.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"

#include "HodEngine/Core/Memory/MallocAllocator.hpp"
#include "HodEngine/Core/Memory/MemLeakDetectorAllocator.hpp"
#include "HodEngine/Core/Memory/Config.hpp"

namespace hod
{
#if defined(HOD_ENABLED_MEMLEAK_DETECTOR)
	using DefaultAllocatorType = MemLeakDetectorAllocator;
#else
	using DefaultAllocatorType = MallocAllocator;
#endif

	#pragma warning(disable:4075)
	#pragma init_seg(".CRT$XCA-000")
	DefaultAllocator DefaultAllocator::_defaultAllocator;
	#pragma warning(default:4075)

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
