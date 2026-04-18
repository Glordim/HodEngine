#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/RHI/PresentationSurface.hpp"

#include "HodEngine/Core/Vector.hpp"

namespace hod::window
{
	class MacOsWindow;
}

namespace CA
{
	class MetalLayer;
	class MetalDrawable;
}

namespace hod::renderer
{
	/// @brief
	class HOD_RENDERER_API MetalPresentationSurface : public PresentationSurface
	{
	public:
		MetalPresentationSurface(window::MacOsWindow* window);
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
		void SetupLayer(window::MacOsWindow* window);

	private:
		CA::MetalLayer*    _layer = nullptr;
		CA::MetalDrawable* _drawable = nullptr;
	};
}
