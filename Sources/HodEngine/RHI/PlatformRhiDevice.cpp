#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/PlatformRhiDevice.hpp"

#if defined(RHI_VULKAN)
	#include "HodEngine/RHI/Vulkan/VulkanRhiDevice.hpp"
	#define PlatformRhiDevice hod::VulkanRhiDevice
#elif defined(RHI_METAL)
	#include "HodEngine/RHI/Metal/MetalRhiDevice.hpp"
	#define PlatformRhiDevice hod::MetalRhiDevice
#elif defined(RHI_D3D12)
	#include "HodEngine/RHI/D3d12/D3d12RhiDevice.hpp"
	#define PlatformRhiDevice hod::D3d12RhiDevice
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
