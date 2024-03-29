#pragma once

#include <vector>

#include "HodEngine/Renderer/RHI/GpuDevice.hpp"

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class GpuDeviceHelper
		{
		public:
			static bool GetAvailableDevices(std::vector<GpuDevice*>* availableDevices);
			static bool GetBestAvailableAndCompatibleDevice(GpuDevice** bestDevice);
		};
	}
}
