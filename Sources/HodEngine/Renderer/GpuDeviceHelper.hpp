#pragma once
#include <HodEngine/HodEngine.hpp>

#include <vector>

#include "HodEngine/Renderer/RHI/GpuDevice.hpp"

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class HOD_API GpuDeviceHelper
		{
		public:
			static bool GetAvailableDevices(std::vector<GpuDevice*>* availableDevices);
			static bool GetBestAvailableAndCompatibleDevice(GpuDevice** bestDevice);
		};
	}
}
