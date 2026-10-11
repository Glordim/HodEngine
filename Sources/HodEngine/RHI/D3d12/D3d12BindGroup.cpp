#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/D3d12/D3d12BindGroup.hpp"

#include "HodEngine/RHI/ShaderSetDescriptor.hpp"

#include "HodEngine/RHI/D3d12/D3d12Buffer.hpp"
#include "HodEngine/RHI/D3d12/D3d12GraphicsPipeline.hpp"
#include "HodEngine/RHI/D3d12/D3d12RhiDevice.hpp"
#include "HodEngine/RHI/D3d12/D3d12Sampler.hpp"
#include "HodEngine/RHI/D3d12/D3d12Texture.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

namespace hod::inline rhi
{
	/// @brief
	D3d12BindGroup::~D3d12BindGroup()
	{
		if (_textureTableIndex != D3d12DescriptorHeap::InvalidIndex)
		{
			D3d12RhiDevice* rhiDevice = D3d12RhiDevice::GetInstance();
			rhiDevice->DeferFree(rhiDevice->GetShaderResourceHeap(), _textureTableIndex, _textureCount);
		}
	}

	/// @brief
	/// @param graphicsPipeline
	/// @param set
	/// @param uniformBuffers one per uniform block of the set
	/// @param uniformBufferCount
	/// @param textureBindings one per texture block of the set
	/// @param textureBindingCount
	/// @return
	bool D3d12BindGroup::Build(const D3d12GraphicsPipeline& graphicsPipeline, uint32_t set, Buffer* const* uniformBuffers, uint32_t uniformBufferCount,
	                           const TextureBinding* textureBindings, uint32_t textureBindingCount)
	{
		const std::map<uint32_t, ShaderSetDescriptor*>& setDescriptors = graphicsPipeline.GetSetDescriptors();
		auto                                            it = setDescriptors.find(set);
		const D3d12GraphicsPipeline::SetLayout*         setLayout = graphicsPipeline.GetSetLayout(set);
		if (it == setDescriptors.end() || setLayout == nullptr)
		{
			OUTPUT_ERROR("D3d12: BindGroup, the pipeline has no set {}", set);
			return false;
		}

		const Vector<ShaderSetDescriptor::BlockUbo>&     uboBlocks = it->second->GetUboBlocks();
		const Vector<ShaderSetDescriptor::BlockTexture>& textureBlocks = it->second->GetTextureBlocks();
		if (uniformBufferCount != uboBlocks.Size() || textureBindingCount != textureBlocks.Size())
		{
			OUTPUT_ERROR("D3d12: BindGroup, set {} expects {} uniform buffers and {} textures, got {} and {}", set, uboBlocks.Size(), textureBlocks.Size(), uniformBufferCount,
			             textureBindingCount);
			return false;
		}

		D3d12RhiDevice* rhiDevice = D3d12RhiDevice::GetInstance();

		_uniformBufferAddresses.Resize(uniformBufferCount);
		for (uint32_t i = 0; i < uniformBufferCount; ++i)
		{
			if (uniformBuffers[i] == nullptr)
			{
				OUTPUT_ERROR("D3d12: BindGroup, no uniform buffer given for \"{}\"", uboBlocks[i]._name);
				return false;
			}
			_uniformBufferAddresses[i] = static_cast<const D3d12Buffer*>(uniformBuffers[i])->GetGpuAddress(); // the offset is given when the set is bound
		}

		if (setLayout->_textureCount > 0)
		{
			_textureTableIndex = rhiDevice->GetShaderResourceHeap().Allocate(setLayout->_textureCount);
			if (_textureTableIndex == D3d12DescriptorHeap::InvalidIndex)
			{
				OUTPUT_ERROR("D3d12: BindGroup, the shader resource heap is full!");
				return false;
			}
			_textureCount = setLayout->_textureCount;
		}

		Vector<const D3d12Sampler*> samplers;
		samplers.Reserve(setLayout->_samplerCount);

		// The descriptors of each table are in the order of their blocks, see D3d12GraphicsPipeline::BuildRootSignature
		uint32_t textureIndex = 0;

		const D3d12Texture* fallbackTexture = static_cast<const D3d12Texture*>(rhiDevice->GetFallbackTexture());
		for (uint32_t i = 0; i < textureBindingCount; ++i)
		{
			const ShaderSetDescriptor::BlockTexture& textureBlock = textureBlocks[i];
			const TextureBinding&                    textureBinding = textureBindings[i];

			const D3d12Texture* texture = textureBinding._texture != nullptr ? static_cast<const D3d12Texture*>(textureBinding._texture) : fallbackTexture;
			if (texture->GetResource() == nullptr) // never built
			{
				texture = fallbackTexture;
			}

			if (textureBlock._type == ShaderSetDescriptor::BlockTexture::Type::Sampler)
			{
				const Sampler* sampler = textureBinding._sampler != nullptr ? textureBinding._sampler : texture->GetDefaultSampler();
				if (sampler == nullptr)
				{
					sampler = rhiDevice->GetSampler(Sampler::CreateInfo());
				}
				samplers.PushBack(static_cast<const D3d12Sampler*>(sampler));
			}
			else
			{
				rhiDevice->GetDevice()->CreateShaderResourceView(texture->GetResource(), nullptr,
				                                                 rhiDevice->GetShaderResourceHeap().GetCpuHandle(_textureTableIndex + textureIndex));
				++textureIndex;
			}
		}

		if (samplers.Empty() == false && rhiDevice->GetSamplerTable(samplers.Data(), samplers.Size(), _samplerTable) == false)
		{
			return false;
		}

		return true;
	}

	/// @brief
	/// @return
	const Vector<D3D12_GPU_VIRTUAL_ADDRESS>& D3d12BindGroup::GetUniformBufferAddresses() const
	{
		return _uniformBufferAddresses;
	}

	/// @brief
	/// @return
	D3D12_GPU_DESCRIPTOR_HANDLE D3d12BindGroup::GetTextureTable() const
	{
		return D3d12RhiDevice::GetInstance()->GetShaderResourceHeap().GetGpuHandle(_textureTableIndex);
	}

	/// @brief
	/// @return
	D3D12_GPU_DESCRIPTOR_HANDLE D3d12BindGroup::GetSamplerTable() const
	{
		return _samplerTable;
	}
}
