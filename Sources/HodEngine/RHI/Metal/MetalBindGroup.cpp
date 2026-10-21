#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Metal/MetalBindGroup.hpp"

#include "HodEngine/RHI/ShaderSetDescriptor.hpp"

#include "HodEngine/RHI/Metal/MetalBuffer.hpp"
#include "HodEngine/RHI/Metal/MetalGraphicsPipeline.hpp"
#include "HodEngine/RHI/Metal/MetalRhiDevice.hpp"
#include "HodEngine/RHI/Metal/MetalSampler.hpp"
#include "HodEngine/RHI/Metal/MetalTexture.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <Metal/Metal.hpp>

#include <HodEngine/Core/Math.hpp>
#include <cstring>

namespace hod::inline rhi
{
	/// @brief
	/// @param graphicsPipeline
	/// @param set
	/// @param uniformBuffers one per uniform block of the set
	/// @param uniformBufferCount
	/// @param textureBindings one per texture block of the set
	/// @param textureBindingCount
	/// @return
	bool MetalBindGroup::Build(const MetalGraphicsPipeline& graphicsPipeline, uint32_t set, Buffer* const* uniformBuffers, uint32_t uniformBufferCount,
	                           const TextureBinding* textureBindings, uint32_t textureBindingCount)
	{
		const std::map<uint32_t, ShaderSetDescriptor*>& setDescriptors = graphicsPipeline.GetSetDescriptors();
		auto                                            it = setDescriptors.find(set);
		if (it == setDescriptors.end())
		{
			OUTPUT_ERROR("Metal: BindGroup, the pipeline has no set {}", set);
			return false;
		}

		const Vector<ShaderSetDescriptor::BlockUbo>&     uboBlocks = it->second->GetUboBlocks();
		const Vector<ShaderSetDescriptor::BlockTexture>& textureBlocks = it->second->GetTextureBlocks();
		if (uniformBufferCount != uboBlocks.Size() || textureBindingCount != textureBlocks.Size())
		{
			OUTPUT_ERROR("Metal: BindGroup, set {} expects {} uniform buffers and {} textures, got {} and {}", set, uboBlocks.Size(), textureBlocks.Size(), uniformBufferCount,
			             textureBindingCount);
			return false;
		}

		MetalRhiDevice* rhiDevice = MetalRhiDevice::GetInstance();

		_vertexBufferIndex = it->second->GetStageSpace(Shader::ShaderType::Vertex);
		_fragmentBufferIndex = it->second->GetStageSpace(Shader::ShaderType::Fragment);

		// Each block is one field of the argument buffer, its binding is the offset of the field
		uint32_t argumentBufferSize = 0;
		for (const ShaderSetDescriptor::BlockUbo& ubo : uboBlocks)
		{
			argumentBufferSize = Max(argumentBufferSize, ubo._binding + (uint32_t)sizeof(MTL::GPUAddress));
		}
		for (const ShaderSetDescriptor::BlockTexture& textureBlock : textureBlocks)
		{
			argumentBufferSize = Max(argumentBufferSize, textureBlock._binding + (uint32_t)sizeof(MTL::ResourceID));
		}
		_argumentBuffer.Resize(argumentBufferSize, 0);

		_uniformBuffers.Resize(uniformBufferCount);
		for (uint32_t i = 0; i < uniformBufferCount; ++i)
		{
			if (uniformBuffers[i] == nullptr)
			{
				OUTPUT_ERROR("Metal: BindGroup, no uniform buffer given for \"{}\"", uboBlocks[i]._name);
				return false;
			}
			_uniformBuffers[i]._buffer = static_cast<const MetalBuffer*>(uniformBuffers[i]);
			_uniformBuffers[i]._offset = uboBlocks[i]._binding;
		}

		const MetalTexture* fallbackTexture = static_cast<const MetalTexture*>(rhiDevice->GetFallbackTexture());
		for (uint32_t i = 0; i < textureBindingCount; ++i)
		{
			const ShaderSetDescriptor::BlockTexture& textureBlock = textureBlocks[i];
			const TextureBinding&                    textureBinding = textureBindings[i];

			const MetalTexture* texture = textureBinding._texture != nullptr ? static_cast<const MetalTexture*>(textureBinding._texture) : fallbackTexture;
			if (texture->GetNativeTexture() == nullptr) // never built
			{
				texture = fallbackTexture;
			}

			MTL::ResourceID resourceId;
			if (textureBlock._type == ShaderSetDescriptor::BlockTexture::Type::Sampler)
			{
				const Sampler* sampler = textureBinding._sampler != nullptr ? textureBinding._sampler : texture->GetDefaultSampler();
				if (sampler == nullptr)
				{
					sampler = rhiDevice->GetSampler(Sampler::CreateInfo());
				}
				if (sampler == nullptr)
				{
					OUTPUT_ERROR("Metal: BindGroup, no sampler for \"{}\"", textureBlock._name);
					return false;
				}
				resourceId = static_cast<const MetalSampler*>(sampler)->GetNativeSampler()->gpuResourceID();
			}
			else
			{
				resourceId = texture->GetNativeTexture()->gpuResourceID();
			}
			std::memcpy(_argumentBuffer.Data() + textureBlock._binding, &resourceId, sizeof(resourceId));
		}

		return true;
	}

	/// @brief
	/// @return
	uint32_t MetalBindGroup::GetVertexBufferIndex() const
	{
		return _vertexBufferIndex;
	}

	/// @brief
	/// @return
	uint32_t MetalBindGroup::GetFragmentBufferIndex() const
	{
		return _fragmentBufferIndex;
	}

	/// @brief
	/// @return
	uint32_t MetalBindGroup::GetArgumentBufferSize() const
	{
		return (uint32_t)_argumentBuffer.Size();
	}

	/// @brief
	/// @return
	uint32_t MetalBindGroup::GetUniformBufferCount() const
	{
		return (uint32_t)_uniformBuffers.Size();
	}

	/// @brief
	/// @param argumentBuffer
	/// @param uniformBufferOffsets
	void MetalBindGroup::WriteArgumentBuffer(void* argumentBuffer, const uint32_t* uniformBufferOffsets) const
	{
		std::memcpy(argumentBuffer, _argumentBuffer.Data(), _argumentBuffer.Size());

		for (uint32_t i = 0; i < _uniformBuffers.Size(); ++i)
		{
			const UniformBuffer& uniformBuffer = _uniformBuffers[i];

			// Read now rather than when the BindGroup is built: resizing a MetalBuffer moves it
			MTL::GPUAddress address = uniformBuffer._buffer->GetNativeBuffer()->gpuAddress() + uniformBufferOffsets[i];
			std::memcpy(static_cast<uint8_t*>(argumentBuffer) + uniformBuffer._offset, &address, sizeof(address));
		}
	}
}
