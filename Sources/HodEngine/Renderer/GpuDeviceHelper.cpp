#include "HodEngine/Renderer/GpuDeviceHelper.hpp"

#include "HodEngine/Renderer/Renderer.hpp"

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool GpuDeviceHelper::GetAvailableDevices(std::vector<GpuDevice*>* availableDevices)
		{
			if (availableDevices == nullptr)
				return false;

			Renderer* renderer = Renderer::GetInstance();

			if (renderer == nullptr)
				return false;

			return renderer->GetAvailableGpuDevices(availableDevices);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool GpuDeviceHelper::GetBestAvailableAndCompatibleDevice(GpuDevice** ret)
		{
			if (ret == nullptr)
				return false;

			Renderer* renderer = Renderer::GetInstance();

			if (renderer == nullptr)
				return false;

			std::vector<GpuDevice*> availableDevices;

			if (renderer->GetAvailableGpuDevices(&availableDevices) == false)
				return false;

			GpuDevice* bestDevice = nullptr;

			size_t deviceCount = availableDevices.size();
			for (size_t i = 0; i < deviceCount; ++i)
			{
				GpuDevice* device = availableDevices[i];

				if (device->compatible == false)
					continue;

				if (bestDevice == nullptr || device->score > bestDevice->score)
					bestDevice = device;
			}

			if (bestDevice == nullptr)
				return false;

			*ret = bestDevice;

			return true;
		}
	}
}
