#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Core/Vector.hpp"

#include "HodEngine/Renderer/RHI/GpuDevice.hpp"

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class HOD_RENDERER_API GpuDeviceHelper
		{
		public:
			static bool GetAvailableDevices(Vector<GpuDevice*>* availableDevices);
			static bool GetBestAvailableAndCompatibleDevice(GpuDevice** bestDevice);
		};
	}
}
