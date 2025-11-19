#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalFence.hpp"
#include "HodEngine/Renderer/RHI/Metal/RendererMetal.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

namespace hod::renderer
{
	/// @brief 
	MetalFence::MetalFence()
		: Fence()
	{
		RendererMetal* metalRenderer = RendererMetal::GetInstance();

		_mtlFence = metalRenderer->GetDevice()->newFence();

		if (_mtlFence == nullptr)
		{
			OUTPUT_ERROR("Metal: Unable to create fence!");
			return;
		}
	}

	/// @brief 
	MetalFence::~MetalFence()
	{
		if (_mtlFence != nullptr)
		{
			_mtlFence->release();
		}
	}

	/// @brief 
	/// @return 
	MTL::Fence* MetalFence::GetNativeFence() const
	{
		return _mtlFence;
	}

	/// @brief 
	/// @return 
	bool MetalFence::Reset()
	{
		return false;
	}

	/// @brief 
	/// @return 
	bool MetalFence::Wait()
	{
		return false;
	}
}
