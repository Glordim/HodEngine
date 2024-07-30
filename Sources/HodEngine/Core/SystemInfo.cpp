#include "HodEngine/Core/pch.hpp"
#include "HodEngine/Core/SystemInfo.hpp"

#if defined(PLATFORM_WINDOWS)
#include <Windows.h>
#elif defined(PLATFORM_LINUX)
#include <sys/sysinfo.h>
#endif

#include <thread>

namespace hod
{
	namespace SystemInfo
	{
		/// @brief 
		/// @return 
		uint32_t ComputeLogicalCoreCount()
		{
			return (uint32_t)std::thread::hardware_concurrency();
			/*
#if defined(PLATFORM_WINDOWS)
			SYSTEM_INFO system_info;
			ZeroMemory(&system_info, sizeof(system_info));

			GetSystemInfo(&system_info);

			return static_cast<size_t>(system_info.dwNumberOfProcessors);
#elif defined(PLATFORM_LINUX)
			return get_nprocs_conf();
#endif
			*/
		}

		/// @brief 
		/// @return 
		uint32_t GetLogicalCoreCount()
		{
			static uint32_t logicalCoreCount = ComputeLogicalCoreCount();
			return logicalCoreCount;
		}
	}
}
