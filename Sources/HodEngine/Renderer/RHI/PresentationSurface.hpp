#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Core/Math/Vector2.hpp"
#include "HodEngine/Core/String.hpp"
#include <cstdint>

namespace hod::window
{
	class Window;
}

namespace hod::renderer
{
	class Semaphore;

	/// @brief
	class HOD_RENDERER_API PresentationSurface
	{
	public:
		PresentationSurface(window::Window* window);
		virtual ~PresentationSurface();

	public:
		virtual void Resize(uint32_t width, uint32_t height);

		virtual bool AcquireNextImageIndex(Semaphore* imageAvailableSemaphore) = 0;
		virtual bool SwapBuffer() = 0;

		void AddSemaphoreToSwapBuffer(const Semaphore* semaphore);

		bool         GetResizeRequested() const;
		virtual bool ApplyResize() = 0;

		virtual Vector2 GetResolution() const = 0;

		window::Window* GetWindow() const;

	protected:
		Vector<const Semaphore*> _semaphoresToSwapBuffer;

		window::Window* _window = nullptr;

		bool     _resizeRequested = false;
		uint32_t _resizeWidth = 0;
		uint32_t _resizeHeight = 0;
	};
}
