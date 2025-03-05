#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <HodEngine/Window/Surface.hpp>

#include <string>
#include <cstdint>

namespace hod::renderer
{
	class Semaphore;

	/// @brief 
	class HOD_RENDERER_API Context : public window::Surface
	{
	public:
						Context() = default;
		virtual			~Context() = default;

	public:

		virtual bool	AcquireNextImageIndex(const Semaphore* signalSemaphore) = 0;
		virtual bool	SwapBuffer(const Semaphore* waitSemaphore) = 0;
	};
}
