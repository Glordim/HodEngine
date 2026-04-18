#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/RHI/PresentationSurface.hpp"

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

namespace hod::inline renderer
{
	/// @brief
	class HOD_RENDERER_API MetalPresentationSurface : public PresentationSurface
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

		void Present();

		CA::MetalLayer* GetLayer() const;

	private:
		void SetupLayer(MacOsWindow* window);

	private:
		CA::MetalLayer*    _layer = nullptr;
		CA::MetalDrawable* _drawable = nullptr;
	};
}
