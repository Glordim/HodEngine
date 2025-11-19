#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/SystemInfo.hpp"

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
