#include "HodEngine/Renderer/RHI/Metal/MetalContext.hpp"
#include "HodEngine/Renderer/RHI/Metal/RendererMetal.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalDevice.hpp"

#include <HodEngine/Core/Output.hpp>

#include <limits>

#include "HodEngine/Renderer/RHI/Metal/MetalCpp/QuartzCore/CAMetalLayer.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalCpp/QuartzCore/CAMetalDrawable.hpp"

namespace hod::renderer
{
	/// @brief 
	MetalContext::MetalContext(window::MacOsWindow* window)
		: Context()
	{
		RendererMetal* rendererMetal = RendererMetal::GetInstance();
		/*
		_layer = new CA::MetalLayer;
    	_layer->device = rendererMetal->GetDevice()->GetNativeDevice();
		*/
		//CreateSemaphores();
		//CreateSwapChain(800, 600);
	}

	/// @brief 
	MetalContext::~MetalContext()
	{
		_drawable->release();
		_layer->release();
	}

	bool MetalContext::AcquireNextImageIndex()
	{
		_drawable = _layer->nextDrawable();
		return true;
	}

	CA::MetalDrawable* MetalContext::GetCurrentDrawable() const
	{
		return _drawable;
	}

	void MetalContext::Resize(uint32_t width, uint32_t height)
	{

	}

	bool MetalContext::SwapBuffer()
	{
		return false;
	}
}
