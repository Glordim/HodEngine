#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/PresentationSurface.hpp"

#include "HodEngine/Core/Vector.hpp"

namespace hod::inline window
{
	class MacOsWindow;
}

namespace CA
{
	class MetalLayer;
	class MetalDrawable;
}

namespace hod::inline rhi
{
	class Texture;

	/// @brief
	class HOD_RHI_API MetalPresentationSurface : public PresentationSurface
	{
	public:
		MetalPresentationSurface(MacOsWindow* window);
		~MetalPresentationSurface() override;

	public:
		bool               AcquireNextImageIndex(Semaphore* imageAvailableSemaphore) override;
		CA::MetalDrawable* GetCurrentDrawable() const;

		void    Resize(uint32_t width, uint32_t height) override;
		Vector2 GetResolution() const override;

		bool SwapBuffer() override;

		CA::MetalLayer* GetLayer() const;

		// Of the size of the drawables, see PresentationSurface::DepthFormat
		Texture* GetDepthTexture() const;

	private:
		void SetupLayer(MacOsWindow* window);

	private:
		CA::MetalLayer*    _layer = nullptr;
		CA::MetalDrawable* _drawable = nullptr;

		// One depth texture serves every drawable: they are drawn into one after the other
		Texture* _depthTexture = nullptr;
	};
}
