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
		void Resize(uint32_t width, uint32_t height) override;

		virtual bool AcquireNextImageIndex(Semaphore* imageAvailableSemaphore) = 0;
		virtual bool SwapBuffer() = 0;

		void AddSemaphoreToSwapBuffer(const Semaphore* semaphore);

		bool         GetResizeRequested() const;
		virtual bool ApplyResize() = 0;

	protected:
		Vector<const Semaphore*> _semaphoresToSwapBuffer;

		bool     _resizeRequested = false;
		uint32_t _resizeWidth = 0;
		uint32_t _resizeHeight = 0;
	};
}
