#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Metal/MetalSampler.hpp"

#include "HodEngine/RHI/Metal/MetalRhiDevice.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <Metal/Metal.hpp>

namespace hod::inline rhi
{
	/// @brief
	/// @param createInfo
	MetalSampler::MetalSampler(const CreateInfo& createInfo)
	: Sampler(createInfo)
	{
	}

	/// @brief
	MetalSampler::~MetalSampler()
	{
		if (_samplerState != nullptr)
		{
			_samplerState->release();
		}
	}

	/// @brief
	/// @return
	bool MetalSampler::Build()
	{
		const CreateInfo& createInfo = GetCreateInfo();

		MTL::SamplerAddressMode  addressMode = createInfo._wrapMode == WrapMode::Clamp ? MTL::SamplerAddressModeClampToEdge : MTL::SamplerAddressModeRepeat;
		MTL::SamplerMinMagFilter filter = createInfo._filterMode == FilterMode::Linear ? MTL::SamplerMinMagFilterLinear : MTL::SamplerMinMagFilterNearest;
		MTL::SamplerMipFilter    mipFilter = createInfo._filterMode == FilterMode::Linear ? MTL::SamplerMipFilterLinear : MTL::SamplerMipFilterNearest;

		MTL::SamplerDescriptor* samplerDescriptor = MTL::SamplerDescriptor::alloc()->init();
		samplerDescriptor->setMinFilter(filter);
		samplerDescriptor->setMagFilter(filter);
		samplerDescriptor->setMipFilter(mipFilter); // NotMipmapped by default: the level 0 only
		samplerDescriptor->setSAddressMode(addressMode);
		samplerDescriptor->setTAddressMode(addressMode);
		samplerDescriptor->setRAddressMode(addressMode);
		samplerDescriptor->setSupportArgumentBuffers(true); // the shaders get their samplers from the argument buffer of a set, see MetalBindGroup
		_samplerState = MetalRhiDevice::GetInstance()->GetDevice()->newSamplerState(samplerDescriptor);
		samplerDescriptor->release();

		if (_samplerState == nullptr)
		{
			OUTPUT_ERROR("Metal: Failed to create texture sampler!");
			return false;
		}

		return true;
	}

	/// @brief
	/// @return
	MTL::SamplerState* MetalSampler::GetNativeSampler() const
	{
		return _samplerState;
	}
}
