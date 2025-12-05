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
		_onWindowResizeDelegate = _window->OnResizeEvent().Add([this](uint16_t width, uint16_t height) { Resize(width, height); });
	}

	PresentationSurface::~PresentationSurface()
	{
		if (_window)
		{
			_window->OnResizeEvent().Remove(_onWindowResizeDelegate);
		}
	}

	void PresentationSurface::AddSemaphoreToSwapBuffer(const Semaphore* semaphore)
	{
		_semaphoresToSwapBuffer.push_back(semaphore);
	}

	void PresentationSurface::Resize(uint32_t width, uint32_t height)
	{
		if (width != _resizeWidth || height != _resizeHeight)
		{
			_resizeRequested = true;
			_resizeWidth = width;
			_resizeHeight = height;
		}
	}

	bool PresentationSurface::GetResizeRequested() const
	{
		return _resizeRequested;
	}

	window::Window* PresentationSurface::GetWindow() const
	{
		return _window;
	}
}
