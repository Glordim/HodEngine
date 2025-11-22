#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <HodEngine/Window/Surface.hpp>

#include "HodEngine/Core/String.hpp"
#include <cstdint>

namespace hod::renderer
{
	class Semaphore;

	/// @brief
	class HOD_RENDERER_API Context : public window::Surface
	{
	public:
		Context();
		virtual ~Context();

	public:
		virtual bool AcquireNextImageIndex(Semaphore* imageAvailableSemaphore) = 0;
		virtual bool SwapBuffer() = 0;

		void AddSemaphoreToSwapBuffer(const Semaphore* semaphore);

	protected:
		Vector<const Semaphore*> _semaphoresToSwapBuffer;
	};
}
