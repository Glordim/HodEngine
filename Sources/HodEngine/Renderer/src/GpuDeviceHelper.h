#pragma once

#include <vector>

#include "RHI/GpuDevice.h"

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
