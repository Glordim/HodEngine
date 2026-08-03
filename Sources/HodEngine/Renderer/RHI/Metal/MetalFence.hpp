#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/RHI/Fence.hpp"

#include <Metal/Metal.hpp>

namespace hod::inline renderer
{
	/// @brief
	class HOD_RENDERER_API MetalFence : public Fence
	{
	public:

						MetalFence();
						MetalFence(const MetalFence&) = delete;
						MetalFence(MetalFence&&) = delete;
						~MetalFence() override;

		MetalFence&		operator=(const MetalFence&) = delete;
		MetalFence&		operator=(MetalFence&&) = delete;

	public:

		bool			Reset() override;
		bool			Wait() override;

		MTL::SharedEvent*	GetNativeEvent() const;
		uint64_t			GetTargetValue() const;

	private:

		MTL::SharedEvent*	_mtlEvent = nullptr;
		uint64_t			_value = 0;
	};
}
