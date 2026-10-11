#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/Sampler.hpp"

#include "HodEngine/RHI/D3d12/D3d12Api.hpp"

namespace hod::inline rhi
{
	/// @brief A D3D12 sampler is a descriptor, there is no object to own:
	/// the descriptors are written by D3d12RhiDevice::GetSamplerTable, from the description kept here.
	class HOD_RHI_API D3d12Sampler : public Sampler
	{
	public:
		D3d12Sampler(const CreateInfo& createInfo);
		~D3d12Sampler() override = default;

		const D3D12_SAMPLER_DESC& GetSamplerDesc() const;

	private:
		D3D12_SAMPLER_DESC _samplerDesc = {};
	};
}
