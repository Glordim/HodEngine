#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Metal/MetalPresentationSurface.hpp"
#include "HodEngine/RHI/Metal/MetalRhiDevice.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"

#include <limits>

#include <Metal/Metal.hpp>
#include <QuartzCore/CAMetalDrawable.hpp>
#include <QuartzCore/CAMetalLayer.hpp>

#include "HodEngine/Window/Desktop/MacOs/MacOsWindow.hpp"

namespace hod::inline rhi
{
	/// @brief
	MetalPresentationSurface::MetalPresentationSurface(MacOsWindow* window)
	: PresentationSurface(window)
	{
		MetalRhiDevice* metalRhiDevice = MetalRhiDevice::GetInstance();

		_layer = CA::MetalLayer::layer();
		_layer->setDevice(metalRhiDevice->GetDevice());
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

	/// @brief The command buffers committed from now on wait for the drawable to be available: no semaphore needed, they are on the same queue
	bool MetalPresentationSurface::AcquireNextImageIndex(Semaphore* /*imageAvailableSemaphore*/)
	{
		if (_drawable != nullptr)
		{
			_drawable->release();
		}
		_drawable = _layer->nextDrawable();
		if (_drawable == nullptr)
		{
			return false;
		}

		MetalRhiDevice::GetInstance()->GetCommandQueue()->wait(_drawable);
		return true;
	}

	CA::MetalDrawable* MetalPresentationSurface::GetCurrentDrawable() const
	{
		return _drawable;
	}

	void MetalPresentationSurface::Resize(uint32_t width, uint32_t height)
	{
		float scaleFactor = static_cast<MacOsWindow*>(_window)->GetScaleFactor();

		//_layer->setContentsScale(scaleFactor);

		CGSize Size;
		Size.width = width * scaleFactor;
		Size.height = height * scaleFactor;
		_layer->setDrawableSize(Size);
	}

	Vector2 MetalPresentationSurface::GetResolution() const
	{
		return Vector2(static_cast<uint32_t>(_layer->drawableSize().width), static_cast<uint32_t>(_layer->drawableSize().height));
	}

	/// @brief Presents once everything drawn into the drawable is committed: the queue signals it after the work committed before.
	/// The semaphores of the RenderViews are not waited for, the queue already orders the signal after them.
	bool MetalPresentationSurface::SwapBuffer()
	{
		_semaphoresToSwapBuffer.Clear();

		if (_drawable == nullptr)
		{
			return false;
		}

		MetalRhiDevice::GetInstance()->GetCommandQueue()->signalDrawable(_drawable);
		_drawable->present();
		return true;
	}

	CA::MetalLayer* MetalPresentationSurface::GetLayer() const
	{
		return _layer;
	}
}
