#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/D3d12/D3d12Sampler.hpp"

namespace hod::inline rhi
{
	/// @brief
	/// @param createInfo
	D3d12Sampler::D3d12Sampler(const CreateInfo& createInfo)
	: Sampler(createInfo)
	{
		D3D12_TEXTURE_ADDRESS_MODE addressMode = createInfo._wrapMode == WrapMode::Clamp ? D3D12_TEXTURE_ADDRESS_MODE_CLAMP : D3D12_TEXTURE_ADDRESS_MODE_WRAP;

		_samplerDesc.Filter = createInfo._filterMode == FilterMode::Linear ? D3D12_FILTER_MIN_MAG_MIP_LINEAR : D3D12_FILTER_MIN_MAG_MIP_POINT;
		_samplerDesc.AddressU = addressMode;
		_samplerDesc.AddressV = addressMode;
		_samplerDesc.AddressW = addressMode;
		_samplerDesc.MipLODBias = 0.0f;
		_samplerDesc.MaxAnisotropy = 1;
		_samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		_samplerDesc.MinLOD = 0.0f;
		_samplerDesc.MaxLOD = 0.0f;
	}

	/// @brief
	/// @return
	const D3D12_SAMPLER_DESC& D3d12Sampler::GetSamplerDesc() const
	{
		return _samplerDesc;
	}
}
