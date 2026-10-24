#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Metal/MetalPresentationSurface.hpp"
#include "HodEngine/RHI/Metal/MetalAutoreleaseScope.hpp"
#include "HodEngine/RHI/Metal/MetalRhiDevice.hpp"
#include "HodEngine/RHI/Texture.hpp"

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
		MetalAutoreleaseScope autoreleaseScope;

		MetalRhiDevice* metalRhiDevice = MetalRhiDevice::GetInstance();

		_layer = CA::MetalLayer::layer()->retain(); // autoreleased
		_layer->setDevice(metalRhiDevice->GetDevice());
		_layer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
		_layer->setFramebufferOnly(true);

		SetupLayer(window);

		Resize(window->GetWidth(), window->GetHeight());
	}

	/// @brief
	MetalPresentationSurface::~MetalPresentationSurface()
	{
		DefaultAllocator::GetInstance().Delete(_depthTexture);

		if (_drawable != nullptr)
		{
			_drawable->release();
		}
		_layer->release();
	}

	/// @brief The command buffers committed from now on wait for the drawable to be available: no semaphore needed, they are on the same queue
	bool MetalPresentationSurface::AcquireNextImageIndex(Semaphore* /*imageAvailableSemaphore*/)
	{
		MetalAutoreleaseScope autoreleaseScope;

		if (_drawable != nullptr)
		{
			_drawable->release();
		}
		_drawable = _layer->nextDrawable(); // autoreleased, kept until the next one
		if (_drawable == nullptr)
		{
			return false;
		}
		_drawable->retain();

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

		// Built again at the size of the drawables, the previous one kept until the frames in flight are done with it
		uint32_t drawableWidth = static_cast<uint32_t>(Size.width);
		uint32_t drawableHeight = static_cast<uint32_t>(Size.height);
		if (drawableWidth == 0 || drawableHeight == 0)
		{
			return; // no drawable to draw into, see AcquireNextImageIndex
		}

		if (_depthTexture == nullptr)
		{
			_depthTexture = MetalRhiDevice::GetInstance()->CreateTexture();
		}
		else if (_depthTexture->GetWidth() == drawableWidth && _depthTexture->GetHeight() == drawableHeight)
		{
			return;
		}

		Texture::CreateInfo depthCreateInfo;
		depthCreateInfo._width = drawableWidth;
		depthCreateInfo._height = drawableHeight;
		depthCreateInfo._format = DepthFormat;
		depthCreateInfo._usage = TextureUsage::RenderTarget;
		if (_depthTexture->Build(depthCreateInfo) == false)
		{
			OUTPUT_ERROR("Metal: Unable to create the depth texture of the presentation surface!");
		}
	}

	/// @brief
	/// @return
	Texture* MetalPresentationSurface::GetDepthTexture() const
	{
		return _depthTexture;
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

		MetalAutoreleaseScope autoreleaseScope;

		MetalRhiDevice::GetInstance()->GetCommandQueue()->signalDrawable(_drawable);
		_drawable->present();
		return true;
	}

	CA::MetalLayer* MetalPresentationSurface::GetLayer() const
	{
		return _layer;
	}
}
