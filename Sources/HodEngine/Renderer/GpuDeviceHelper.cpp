#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/GpuDeviceHelper.hpp"

#include "HodEngine/RHI/RhiDevice.hpp"

namespace hod::inline renderer
{
	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool GpuDeviceHelper::GetAvailableDevices(Vector<GpuDevice*>* availableDevices)
	{
		if (availableDevices == nullptr)
		{
			return false;
		}

		RhiDevice* rhiDevice = RhiDevice::GetInstance();

		if (rhiDevice == nullptr)
		{
			return false;
		}

		return rhiDevice->GetAvailableGpuDevices(availableDevices);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool GpuDeviceHelper::GetBestAvailableAndCompatibleDevice(GpuDevice** ret)
	{
		if (ret == nullptr)
		{
			return false;
		}

		RhiDevice* rhiDevice = RhiDevice::GetInstance();

		if (rhiDevice == nullptr)
		{
			return false;
		}

		Vector<GpuDevice*> availableDevices;

		if (rhiDevice->GetAvailableGpuDevices(&availableDevices) == false)
		{
			return false;
		}

		GpuDevice* bestDevice = nullptr;

		uint32_t deviceCount = availableDevices.Size();
		for (uint32_t i = 0; i < deviceCount; ++i)
		{
			GpuDevice* device = availableDevices[i];

			if (device->compatible == false)
			{
				continue;
			}

			if (bestDevice == nullptr || device->score > bestDevice->score)
			{
				bestDevice = device;
			}
		}

		if (bestDevice == nullptr)
		{
			return false;
		}

		*ret = bestDevice;

		return true;
	}
}
