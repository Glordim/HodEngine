#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Core/Event.hpp"
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
		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual bool AcquireNextImageIndex(Semaphore* imageAvailableSemaphore) = 0;
		virtual bool SwapBuffer() = 0;

		void AddSemaphoreToSwapBuffer(Semaphore* semaphore);

		virtual Vector2 GetResolution() const = 0;

		window::Window* GetWindow() const;

	protected:
		Vector<Semaphore*> _semaphoresToSwapBuffer;

		Delegate _onWindowResizeDelegate;

		window::Window* _window = nullptr;
	};
}
