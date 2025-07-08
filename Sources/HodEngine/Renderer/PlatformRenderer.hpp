#pragma once

#if defined(RENDERER_VULKAN)

	#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.hpp"
	#define PlatformRenderer hod::renderer::RendererVulkan

#elif defined(RENDERER_METAL)

	#include "HodEngine/Renderer/RHI/Metal/RendererMetal.hpp"
	#define PlatformRenderer hod::renderer::RendererMetal

#elif defined(RENDERER_D3D12)

	#include "HodEngine/Renderer/RHI/D3d12/RendererDirectX12.hpp"
	#define PlatformRenderer hod::renderer::RendererDirectX12

#endif
