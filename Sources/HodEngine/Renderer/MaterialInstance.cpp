#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/MaterialInstance.hpp"

#include "HodEngine/Renderer/FrameResources.hpp"
#include "HodEngine/Renderer/Material.hpp"
#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/UniformAllocator.hpp"

#include "HodEngine/RHI/BindGroup.hpp"
#include "HodEngine/RHI/CommandBuffer.hpp"
#include "HodEngine/RHI/RhiDevice.hpp"
#include "HodEngine/RHI/ShaderSetDescriptor.hpp"
#include "HodEngine/RHI/Texture.hpp"

#include <HodEngine/Core/Assert.hpp>
#include <HodEngine/Core/StaticArray.hpp>

#include <cstring>

namespace hod::inline renderer
{
	/// @brief
	/// @param material
	/// @return
	MaterialInstance* MaterialInstance::Create(const Material* material)
	{
		if (material == nullptr)
		{
			return nullptr;
		}
		return DefaultAllocator::GetInstance().New<MaterialInstance>(*material);
	}

	/// @brief
	/// @param material
	MaterialInstance::MaterialInstance(const Material& material)
	: _material(material)
	{
		const std::map<uint32_t, ShaderSetDescriptor*>& setDescriptors = material.GetSetDescriptors();

		// The sets of a material are contiguous from 0
		_sets.Resize((uint32_t)setDescriptors.size());
		for (const auto& setPair : setDescriptors)
		{
			Set& set = _sets[setPair.first];

			const Vector<ShaderSetDescriptor::BlockUbo>& uboBlocks = setPair.second->GetUboBlocks();
			Assert(uboBlocks.Size() <= MaxUniformBlocksPerSet);
			set._uniformBlocks.Resize(uboBlocks.Size());
			for (uint32_t blockIndex = 0; blockIndex < uboBlocks.Size(); ++blockIndex)
			{
				const ShaderSetDescriptor::BlockUbo::Member& rootMember = uboBlocks[blockIndex]._rootMember;
				set._uniformBlocks[blockIndex].Resize((uint32_t)(rootMember._size * rootMember._count), 0);
			}

			set._textures.Resize(setPair.second->GetTextureBlocks().Size());
		}
	}

	/// @brief
	MaterialInstance::~MaterialInstance()
	{
		for (Set& set : _sets)
		{
			ReleaseBindGroups(set);
		}
	}

	/// @brief
	/// @return
	const Material& MaterialInstance::GetMaterial() const
	{
		return _material;
	}

	/// @brief
	void MaterialInstance::SetInt(const String& memberName, int value)
	{
		SetUniform(memberName, &value, sizeof(value));
	}

	/// @brief
	void MaterialInstance::SetFloat(const String& memberName, float value)
	{
		SetUniform(memberName, &value, sizeof(value));
	}

	/// @brief
	void MaterialInstance::SetVec2(const String& memberName, const Vector2& value)
	{
		SetUniform(memberName, &value, sizeof(value));
	}

	/// @brief
	void MaterialInstance::SetVec4(const String& memberName, const Vector4& value)
	{
		SetUniform(memberName, &value, sizeof(value));
	}

	/// @brief
	void MaterialInstance::SetMat4(const String& memberName, const Matrix4& value)
	{
		SetUniform(memberName, &value, sizeof(value));
	}

	/// @brief
	/// @param path
	/// @param value
	/// @param size
	void MaterialInstance::SetUniform(const String& path, const void* value, uint32_t size)
	{
		Material::UniformLocation location;
		if (_material.FindUniform(path, location) == false || location._size != size)
		{
			return;
		}

		Vector<uint8_t>& block = _sets[location._set]._uniformBlocks[location._block];
		if (location._offset + size > block.Size())
		{
			return;
		}
		std::memcpy(block.Data() + location._offset, value, size);
	}

	/// @brief
	/// @param memberName
	/// @param value
	void MaterialInstance::SetTexture(const String& memberName, const Texture* value)
	{
		// "No texture" means white, so that a material multiplying its texture by a color draws that color alone
		if (value == nullptr || value->GetWidth() == 0)
		{
			value = Renderer::GetInstance()->GetWhiteTexture();
		}

		SetTextureSlot(memberName, value, value->GetDefaultSampler());
		SetTextureSlot(memberName + "Sampler", value, value->GetDefaultSampler());
	}

	/// @brief
	/// @param memberName
	/// @param value
	void MaterialInstance::SetSampler(const String& memberName, const Sampler* value)
	{
		Material::TextureLocation location;
		if (_material.FindTexture(memberName, location) == false)
		{
			return;
		}

		SetTextureSlot(memberName, _sets[location._set]._textures[location._block]._texture, value);
	}

	/// @brief
	/// @param name
	/// @param texture
	/// @param sampler
	void MaterialInstance::SetTextureSlot(const String& name, const Texture* texture, const Sampler* sampler)
	{
		Material::TextureLocation location;
		if (_material.FindTexture(name, location) == false)
		{
			return;
		}

		Set&         set = _sets[location._set];
		TextureSlot& slot = set._textures[location._block];
		if (slot._set == false || slot._texture != texture || slot._sampler != sampler)
		{
			slot._texture = texture;
			slot._sampler = sampler;
			slot._set = true;

			// A BindGroup is immutable: the ones made with the previous texture or sampler are of no use anymore
			ReleaseBindGroups(set);
		}
	}

	/// @brief
	/// @param commandBuffer
	/// @param setOffset
	/// @param setCount
	void MaterialInstance::Bind(CommandBuffer& commandBuffer, uint32_t setOffset, uint32_t setCount) const
	{
		commandBuffer.SetGraphicsPipeline(_material.GetGraphicsPipeline());

		UniformAllocator& uniformAllocator = Renderer::GetInstance()->GetCurrentFrameResources().GetUniformAllocator();

		for (uint32_t setIndex = setOffset; setIndex < _sets.Size() && setIndex - setOffset < setCount; ++setIndex)
		{
			const Set& set = _sets[setIndex];
			if (set._uniformBlocks.Empty() && set._textures.Empty())
			{
				continue;
			}

			// This frame gets its own copy of the values
			StaticArray<Buffer*, MaxUniformBlocksPerSet>  uniformBuffers;
			StaticArray<uint32_t, MaxUniformBlocksPerSet> uniformBufferOffsets;
			uint32_t                                      uniformBlockCount = (uint32_t)set._uniformBlocks.Size();
			for (uint32_t blockIndex = 0; blockIndex < uniformBlockCount; ++blockIndex)
			{
				const Vector<uint8_t>& block = set._uniformBlocks[blockIndex];

				UniformAllocator::Allocation allocation = uniformAllocator.Allocate((uint32_t)block.Size());
				if (allocation._buffer == nullptr)
				{
					return;
				}
				std::memcpy(allocation._data, block.Data(), block.Size());

				uniformBuffers[blockIndex] = allocation._buffer;
				uniformBufferOffsets[blockIndex] = allocation._offset;
			}

			BindGroup* bindGroup = GetBindGroup(setIndex, uniformBuffers.Data(), uniformBlockCount);
			if (bindGroup == nullptr)
			{
				return;
			}
			commandBuffer.SetBindGroup(setIndex, bindGroup, uniformBufferOffsets.Data(), uniformBlockCount);
		}
	}

	/// @brief The BindGroup of a set for the uniform buffers its values were just uploaded to
	/// @param setIndex
	/// @param uniformBuffers
	/// @param uniformBufferCount
	/// @return
	BindGroup* MaterialInstance::GetBindGroup(uint32_t setIndex, Buffer* const* uniformBuffers, uint32_t uniformBufferCount) const
	{
		const Set& set = _sets[setIndex];

		for (const BoundGroup& boundGroup : set._bindGroups)
		{
			if (std::memcmp(boundGroup._uniformBuffers.Data(), uniformBuffers, sizeof(Buffer*) * uniformBufferCount) == 0)
			{
				return boundGroup._bindGroup;
			}
		}

		// The textures nobody set are left to the fallback of the RHI, which is not meant to be seen: tell about it
		const Vector<ShaderSetDescriptor::BlockTexture>& textureBlocks = _material.GetSetDescriptors().find(setIndex)->second->GetTextureBlocks();

		Vector<BindGroup::TextureBinding> textureBindings;
		textureBindings.Resize(set._textures.Size());
		for (uint32_t blockIndex = 0; blockIndex < set._textures.Size(); ++blockIndex)
		{
			const TextureSlot& slot = set._textures[blockIndex];
			if (slot._set)
			{
				textureBindings[blockIndex]._texture = slot._texture;
				textureBindings[blockIndex]._sampler = slot._sampler;
			}
			else if (textureBlocks[blockIndex]._type != ShaderSetDescriptor::BlockTexture::Sampler)
			{
				_material.ReportUnsetTexture(textureBlocks[blockIndex]._name);
			}
		}

		BoundGroup boundGroup;
		boundGroup._bindGroup = RhiDevice::GetInstance()->CreateBindGroup(_material.GetGraphicsPipeline(), setIndex, uniformBuffers, uniformBufferCount, textureBindings.Data(),
		                                                                  (uint32_t)textureBindings.Size());
		if (boundGroup._bindGroup == nullptr)
		{
			return nullptr;
		}

		boundGroup._uniformBuffers.Resize(uniformBufferCount);
		for (uint32_t index = 0; index < uniformBufferCount; ++index)
		{
			boundGroup._uniformBuffers[index] = uniformBuffers[index];
		}
		set._bindGroups.PushBack(boundGroup);

		return boundGroup._bindGroup;
	}

	/// @brief
	/// @param set
	void MaterialInstance::ReleaseBindGroups(Set& set)
	{
		// The RHI keeps what a frame in flight still uses alive
		for (const BoundGroup& boundGroup : set._bindGroups)
		{
			DefaultAllocator::GetInstance().Delete(boundGroup._bindGroup);
		}
		set._bindGroups.Clear();
	}
}
