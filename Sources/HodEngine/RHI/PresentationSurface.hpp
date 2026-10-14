#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/Core/Event.hpp"
#include "HodEngine/Math/Vector2.hpp"
#include "HodEngine/Core/String.hpp"
#include "HodEngine/RHI/Enums.hpp"
#include <cstdint>

namespace hod::inline window
{
	class Window;
}

namespace hod::inline rhi
{
	class Semaphore;

	/// @brief
	class HOD_RHI_API PresentationSurface
	{
	public:
		// The formats of every presentation surface, what the pipelines drawn into them must be created for
		static constexpr PixelFormat ColorFormat = PixelFormat::R8G8B8A8_UNorm;
		static constexpr PixelFormat DepthFormat = PixelFormat::D16_UNorm;

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

		window::Window* _window = nullptr;
	};
}
