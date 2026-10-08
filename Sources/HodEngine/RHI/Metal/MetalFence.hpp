#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/Fence.hpp"

#include <Metal/Metal.hpp>

namespace hod::inline rhi
{
	/// @brief
	class HOD_RHI_API MetalFence : public Fence
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
