#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/PlatformRenderer.hpp"

#if defined(RENDERER_VULKAN)
	#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.hpp"
	#define PlatformRenderer hod::RendererVulkan
#elif defined(RENDERER_METAL)
	#include "HodEngine/Renderer/RHI/Metal/RendererMetal.hpp"
	#define PlatformRenderer hod::RendererMetal
#elif defined(RENDERER_D3D12)
	#include "HodEngine/Renderer/RHI/D3d12/RendererDirectX12.hpp"
	#define PlatformRenderer hod::RendererDirectX12
#else
	#pragma error
#endif

namespace hod::inline renderer
{
	Renderer* CreatePlatformRenderer()
	{
		return PlatformRenderer::CreateInstance();
	}
}
