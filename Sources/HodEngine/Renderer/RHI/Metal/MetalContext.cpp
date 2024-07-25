#include "HodEngine/Renderer/RHI/Metal/MetalContext.hpp"
#include "HodEngine/Renderer/RHI/Metal/RendererMetal.hpp"

#include <HodEngine/Core/Output.hpp>

#include <limits>

#include "HodEngine/Renderer/RHI/Metal/MetalCpp/QuartzCore/CAMetalLayer.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalCpp/QuartzCore/CAMetalDrawable.hpp"

#include <Metal/Metal.hpp>

#include "HodEngine/Window/Desktop/MacOs/MacOsWindow.hpp"

namespace hod::renderer
{
	/// @brief 
	MetalContext::MetalContext(window::MacOsWindow* window)
		: Context()
		, _window(window)
	{
		RendererMetal* rendererMetal = RendererMetal::GetInstance();

		_layer = CA::MetalLayer::layer();
		_layer->setDevice(rendererMetal->GetDevice());
		_layer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
		_layer->setFramebufferOnly(true);

		Resize(window->GetWidth(), window->GetHeight());
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
		float scaleFactor = _window->GetScaleFactor();

		//_layer->setContentsScale(scaleFactor);

        CGSize size;
        size.width = width * scaleFactor;
        size.height = height * scaleFactor;
        _layer->setDrawableSize(size);
	}

	bool MetalContext::SwapBuffer()
	{
		return false;
	}

	CA::MetalLayer* MetalContext::GetLayer() const
	{
		return _layer;
	}
}
