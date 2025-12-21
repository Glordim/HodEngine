#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/RHI/PresentationSurface.hpp"
#include "HodEngine/Renderer/RHI/Semaphore.hpp"

#include <HodEngine/Core/Vector.hpp>
#include <HodEngine/Window/Window.hpp>

#include <fstream>
#include <iostream>

namespace hod::renderer
{
	PresentationSurface::PresentationSurface(window::Window* window)
	: _window(window)
	{
	}

	PresentationSurface::~PresentationSurface() {}

	void PresentationSurface::AddSemaphoreToSwapBuffer(Semaphore* semaphore)
	{
		_semaphoresToSwapBuffer.push_back(semaphore);
	}

	window::Window* PresentationSurface::GetWindow() const
	{
		return _window;
	}
}
