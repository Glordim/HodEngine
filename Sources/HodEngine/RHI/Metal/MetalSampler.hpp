#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/Sampler.hpp"

namespace MTL
{
	class SamplerState;
}

namespace hod::inline rhi
{
	/// @brief
	class HOD_RHI_API MetalSampler : public Sampler
	{
	public:
		MetalSampler(const CreateInfo& createInfo);
		~MetalSampler() override;

		bool Build();

		MTL::SamplerState* GetNativeSampler() const;

	private:
		MTL::SamplerState* _samplerState = nullptr;
	};
}
