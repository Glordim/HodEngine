#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/RhiDevice.hpp"
#include "HodEngine/RHI/PresentationSurface.hpp"
#include "HodEngine/RHI/Semaphore.hpp"

#include <HodEngine/Core/Vector.hpp>
#include <HodEngine/Window/Window.hpp>

namespace hod::inline rhi
{
	PresentationSurface::PresentationSurface(Window* window)
	: _window(window)
	{
	}

	PresentationSurface::~PresentationSurface() {}

	void PresentationSurface::AddSemaphoreToSwapBuffer(Semaphore* semaphore)
	{
		_semaphoresToSwapBuffer.push_back(semaphore);
	}

	Window* PresentationSurface::GetWindow() const
	{
		return _window;
	}
}
