#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalPresentationSurface.hpp"
#include "HodEngine/Renderer/RHI/Metal/RendererMetal.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"

#include <limits>

#include <Metal/Metal.hpp>
#include <QuartzCore/CAMetalDrawable.hpp>
#include <QuartzCore/CAMetalLayer.hpp>

#include "HodEngine/Window/Desktop/MacOs/MacOsWindow.hpp"

namespace hod::renderer
{
	/// @brief
	MetalPresentationSurface::MetalPresentationSurface(window::MacOsWindow* window)
	: PresentationSurface(window)
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
	MetalPresentationSurface::~MetalPresentationSurface()
	{
		if (_drawable != nullptr)
		{
			_drawable->release();
		}
		_layer->release();
	}

	bool MetalPresentationSurface::AcquireNextImageIndex()
	{
		if (_drawable != nullptr)
		{
			_drawable->release();
		}
		_drawable = _layer->nextDrawable();
		return _drawable != nullptr;
	}

	CA::MetalDrawable* MetalPresentationSurface::GetCurrentDrawable() const
	{
		return _drawable;
	}

	void MetalPresentationSurface::Resize(uint32_t width, uint32_t height)
	{
		float scaleFactor = static_cast<window::MacOsWindow*>(_window)->GetScaleFactor();

		//_layer->setContentsScale(scaleFactor);

		CGSize Size;
		Size.width = width * scaleFactor;
		Size.height = height * scaleFactor;
		_layer->setDrawableSize(Size);
	}

	Vector2 MetalPresentationSurface::GetResolution()
	{
		return Vector2(static_cast<uint32_t>(_layer->drawableSize().width), static_cast<uint32_t>(_layer->drawableSize().height));
	}

	bool MetalPresentationSurface::SwapBuffer()
	{
		return true;
	}

	CA::MetalLayer* MetalPresentationSurface::GetLayer() const
	{
		return _layer;
	}
}
