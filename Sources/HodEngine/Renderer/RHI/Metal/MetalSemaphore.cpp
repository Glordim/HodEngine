#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalSemaphore.hpp"
#include "HodEngine/Renderer/RHI/Metal/RendererMetal.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

namespace hod::renderer
{
	/// @brief 
	MetalSemaphore::MetalSemaphore()
		: Semaphore()
	{
		RendererMetal* metalRenderer = RendererMetal::GetInstance();

		_mtlEvent = metalRenderer->GetDevice()->newEvent();

		if (_mtlEvent == nullptr)
		{
			OUTPUT_ERROR("Metal: Unable to create event!");
			return;
		}
	}

	/// @brief 
	MetalSemaphore::~MetalSemaphore()
	{
		if (_mtlEvent != nullptr)
		{
			_mtlEvent->release();
		}
	}

	/// @brief 
	/// @return 
	MTL::Event* MetalSemaphore::GetNativeSemaphore() const
	{
		return _mtlEvent;
	}
}
