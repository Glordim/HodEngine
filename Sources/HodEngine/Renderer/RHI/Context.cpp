#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Context.hpp"
#include "HodEngine/Renderer/RHI/Semaphore.hpp"
#include "HodEngine/Renderer/Renderer.hpp"

#include "HodEngine/Core/Vector.hpp"
#include <fstream>
#include <iostream>

namespace hod::renderer
{
	Context::Context()
	{
		_imageAvailableSemaphore = Renderer::GetInstance()->CreateSemaphore();
	}

	Context::~Context()
	{
		DefaultAllocator::GetInstance().Delete(_imageAvailableSemaphore);
	}

	const Semaphore* Context::GetImageAvailableSempahore() const
	{
		return _imageAvailableSemaphore;
	}

	void Context::AddSemaphoreToSwapBuffer(const Semaphore* semaphore)
	{
		_semaphoresToSwapBuffer.push_back(semaphore);
	}
}
