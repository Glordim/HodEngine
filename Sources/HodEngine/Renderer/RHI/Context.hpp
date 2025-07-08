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
		virtual			~Context();

	public:

		virtual bool	AcquireNextImageIndex() = 0;
		virtual bool	SwapBuffer() = 0;

		const Semaphore*	GetImageAvailableSempahore() const;
		void				AddSemaphoreToSwapBuffer(const Semaphore* semaphore);

	protected:

		Semaphore*					_imageAvailableSemaphore;
		Vector<const Semaphore*>	_semaphoresToSwapBuffer;
	};
}
