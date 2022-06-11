#include "SystemInfo.h"

#include <Windows.h>

namespace HOD
{
	namespace SystemInfo
	{
		/// @brief 
		/// @return 
		uint32_t ComputeLogicalCoreCount()
		{
			SYSTEM_INFO system_info;
			ZeroMemory(&system_info, sizeof(system_info));

			GetSystemInfo(&system_info);

			return static_cast<size_t>(system_info.dwNumberOfProcessors);
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
