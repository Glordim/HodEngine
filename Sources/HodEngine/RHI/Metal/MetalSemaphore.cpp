#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Metal/MetalSemaphore.hpp"
#include "HodEngine/RHI/Metal/RhiDeviceMetal.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

namespace hod::inline rhi
{
	/// @brief 
	MetalSemaphore::MetalSemaphore()
		: Semaphore()
	{
		RhiDeviceMetal* metalRenderer = RhiDeviceMetal::GetInstance();

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

	/// @brief
	/// @return
	uint64_t MetalSemaphore::IncrementAndGetTargetValue()
	{
		return ++_value;
	}

	/// @brief
	/// @return
	uint64_t MetalSemaphore::GetTargetValue() const
	{
		return _value;
	}
}
