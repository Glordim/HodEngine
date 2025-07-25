#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalContext.hpp"
#include "HodEngine/Renderer/RHI/Metal/RendererMetal.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"

#include <limits>

#include <QuartzCore/CAMetalLayer.hpp>
#include <QuartzCore/CAMetalDrawable.hpp>
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

		SetupLayer(window);

		Resize(window->GetWidth(), window->GetHeight());
	}

	/// @brief 
	MetalContext::~MetalContext()
	{
		if (_drawable != nullptr)
		{
			_drawable->release();
		}
		_layer->release();
	}

	bool MetalContext::AcquireNextImageIndex()
	{
		if (_drawable != nullptr)
		{
			_drawable->release();
		}
		_drawable = _layer->nextDrawable();
		return (_drawable != nullptr);
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

	Vector2 MetalContext::GetResolution()
	{
		return Vector2(static_cast<uint32_t>(_layer->drawableSize().width), static_cast<uint32_t>(_layer->drawableSize().height));
	}

	bool MetalContext::SwapBuffer()
	{
		return true;
	}

	CA::MetalLayer* MetalContext::GetLayer() const
	{
		return _layer;
	}
}
