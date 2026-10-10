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

	/// @brief The device does not need a window: presentation surfaces are created afterwards with CreatePresentationSurface
	/// @param physicalDeviceIdentifier
	/// @return
	bool RhiDevice::Init(uint32_t physicalDeviceIdentifier)
	{
		if (InitDevice(physicalDeviceIdentifier) == false)
		{
			return false;
		}

		// Magenta: a texture slot nobody set is an omission, it has to stand out
		uint8_t pixels[4 * 2 * 2] = {255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255};

		_fallbackTexture = CreateTexture();
		if (_fallbackTexture == nullptr || _fallbackTexture->BuildBuffer(2, 2, pixels, Texture::CreateInfo()) == false)
		{
			OUTPUT_ERROR("RhiDevice: Unable to create the fallback texture!");
			return false;
		}

		return true;
	}

	/// @brief
	void RhiDevice::Clear()
	{
		DefaultAllocator::GetInstance().Delete(_fallbackTexture);
		_fallbackTexture = nullptr;

		for (PresentationSurface* presentationSurface : _presentationSurfaces)
		{
			DefaultAllocator::GetInstance().Delete(presentationSurface);
		}
		_presentationSurfaces.Clear();

		for (Sampler* sampler : _samplers)
		{
			DefaultAllocator::GetInstance().Delete(sampler);
		}
		_samplers.Clear();
	}

	/// @brief
	/// @param createInfo
	/// @return
	const Sampler* RhiDevice::GetSampler(const Sampler::CreateInfo& createInfo)
	{
		for (const Sampler* sampler : _samplers)
		{
			if (sampler->GetCreateInfo() == createInfo)
			{
				return sampler;
			}
		}

		Sampler* sampler = CreateSampler(createInfo);
		if (sampler != nullptr)
		{
			_samplers.PushBack(sampler);
		}
		return sampler;
	}

	/// @brief
	/// @return
	Texture* RhiDevice::GetFallbackTexture() const
	{
		return _fallbackTexture;
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
