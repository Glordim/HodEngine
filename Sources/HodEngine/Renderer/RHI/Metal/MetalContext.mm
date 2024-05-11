#include "HodEngine/Renderer/RHI/Metal/MetalContext.hpp"
#include "HodEngine/Renderer/RHI/Metal/RendererMetal.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalDevice.hpp"

#include <HodEngine/Core/Output.hpp>

#include <limits>

#import <QuartzCore/CAMetalLayer.h>

namespace hod::renderer
{
	/// @brief 
	MetalContext::MetalContext(window::MacOsWindow* window)
		: Context()
	{
		RendererMetal* rendererMetal = RendererMetal::GetInstance();
		_layer = [CAMetalLayer layer];
    	_layer.device = rendererMetal->GetDevice()->GetNativeDevice();
		//CreateSemaphores();
		//CreateSwapChain(800, 600);
	}

	/// @brief 
	MetalContext::~MetalContext()
	{
		
	}
}
