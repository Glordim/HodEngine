#pragma once
#include "HodEngine/Core/Export.hpp"
#include "HodEngine/Core/Memory/Config.hpp"
#include "HodEngine/Core/Memory/MemoryManagerStd.hpp"
#include "HodEngine/Core/Memory/MemoryManagerLeakDetector.hpp"

namespace hod
{
#if defined(HOD_ENABLED_MEMLEAK_DETECTOR)
	using MemoryManager = MemoryManagerLeakDetector;
#else
	using MemoryManager = MemoryManagerStd;
#endif
}
