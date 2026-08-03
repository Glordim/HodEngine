#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalFence.hpp"
#include "HodEngine/Renderer/RHI/Metal/RendererMetal.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

namespace hod::inline renderer
{
	/// @brief
	MetalFence::MetalFence()
		: Fence()
	{
		RendererMetal* metalRenderer = RendererMetal::GetInstance();

		_mtlEvent = metalRenderer->GetDevice()->newSharedEvent();

		if (_mtlEvent == nullptr)
		{
			OUTPUT_ERROR("Metal: Unable to create fence!");
			return;
		}
	}

	/// @brief
	MetalFence::~MetalFence()
	{
		if (_mtlEvent != nullptr)
		{
			_mtlEvent->release();
		}
	}

	/// @brief
	/// @return
	MTL::SharedEvent* MetalFence::GetNativeEvent() const
	{
		return _mtlEvent;
	}

	/// @brief
	/// @return
	uint64_t MetalFence::GetTargetValue() const
	{
		return _value;
	}

	/// @brief
	/// @return
	bool MetalFence::Reset()
	{
		++_value;
		return true;
	}

	/// @brief
	/// @return
	bool MetalFence::Wait()
	{
		return _mtlEvent->waitUntilSignaledValue(_value, UINT64_MAX);
	}
}
