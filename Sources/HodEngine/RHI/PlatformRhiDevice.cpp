#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/PlatformRhiDevice.hpp"

#if defined(RHI_VULKAN)
	#include "HodEngine/RHI/Vulkan/RhiDeviceVulkan.hpp"
	#define PlatformRhiDevice hod::RhiDeviceVulkan
#elif defined(RHI_METAL)
	#include "HodEngine/RHI/Metal/RhiDeviceMetal.hpp"
	#define PlatformRhiDevice hod::RhiDeviceMetal
#elif defined(RHI_D3D12)
	#include "HodEngine/RHI/D3d12/RhiDeviceDirectX12.hpp"
	#define PlatformRhiDevice hod::RhiDeviceDirectX12
#else
	#pragma error
#endif

namespace hod::inline rhi
{
	RhiDevice* CreatePlatformRhiDevice()
	{
		return PlatformRhiDevice::CreateInstance();
	}
}
