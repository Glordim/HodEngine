#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/RhiDevice.hpp"

#include "HodEngine/RHI/PresentationSurface.hpp"
#include "HodEngine/RHI/Texture.hpp"

namespace hod::inline rhi
{
	/// @brief
	_SingletonConstructor(RhiDevice) {}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	RhiDevice::~RhiDevice() {}

	/// @brief
	void RhiDevice::Clear()
	{
		DefaultAllocator::GetInstance().Delete(_defaultWhiteTexture);
		_defaultWhiteTexture = nullptr;

		for (PresentationSurface* presentationSurface : _presentationSurfaces)
		{
			DefaultAllocator::GetInstance().Delete(presentationSurface);
		}
		_presentationSurfaces.Clear();
		_mainPresentationSurface = nullptr;
	}

	/// @brief
	/// @return
	Texture* RhiDevice::GetDefaultWhiteTexture()
	{
		if (_defaultWhiteTexture == nullptr)
		{
			uint8_t pixels[4 * 2 * 2] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

			_defaultWhiteTexture = CreateTexture();
			_defaultWhiteTexture->BuildBuffer(2, 2, pixels, Texture::CreateInfo());
		}
		return _defaultWhiteTexture;
	}

	void RhiDevice::DestroyPresentationSurface(window::Window* window)
	{
		for (auto it = _presentationSurfaces.Begin(); it != _presentationSurfaces.End(); ++it)
		{
			if ((*it)->GetWindow() == window)
			{
				WaitIdle();
				PresentationSurface* presentationSurface = *it;
				_presentationSurfaces.Erase(it);
				if (_mainPresentationSurface == presentationSurface)
				{
					_mainPresentationSurface = nullptr;
				}
				DefaultAllocator::GetInstance().Delete(presentationSurface);
				return;
			}
		}
	}

	PresentationSurface* RhiDevice::FindPresentationSurface(window::Window* window) const
	{
		for (PresentationSurface* presentationSurface : _presentationSurfaces)
		{
			if (presentationSurface->GetWindow() == window)
			{
				return presentationSurface;
			}
		}
		return nullptr;
	}

	PresentationSurface* RhiDevice::GetMainPresentationSurface() const
	{
		return _mainPresentationSurface;
	}

	/// @brief Releases what was deferred the last time this frame slot was used; the caller must have waited for that frame's GPU work to complete.
	void RhiDevice::BeginFrame()
	{
		FlushDeferredDeletions(_frameIndex);
	}

	void RhiDevice::EndFrame()
	{
		++_frameCount;
		_frameIndex = _frameCount % _frameInFlight;
	}

	uint32_t RhiDevice::GetFrameIndex() const
	{
		return _frameIndex;
	}

	uint32_t RhiDevice::GetFrameInFlightCount() const
	{
		return _frameInFlight;
	}
}
