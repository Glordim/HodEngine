#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/RHI/Context.hpp"
#include "HodEngine/Renderer/RHI/Semaphore.hpp"

#include "HodEngine/Core/Vector.hpp"
#include <fstream>
#include <iostream>

namespace hod::renderer
{
	Context::Context() {}

	Context::~Context() {}

	void Context::AddSemaphoreToSwapBuffer(const Semaphore* semaphore)
	{
		_semaphoresToSwapBuffer.push_back(semaphore);
	}

	void Context::Resize(uint32_t width, uint32_t height)
	{
		if (width != _resizeWidth && height != _resizeHeight)
		{
			_resizeRequested = true;
			_resizeWidth = width;
			_resizeHeight = height;
		}
	}

	bool Context::GetResizeRequested() const
	{
		return _resizeRequested;
	}
}
